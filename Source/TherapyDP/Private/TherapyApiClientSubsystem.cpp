#include "TherapyApiClientSubsystem.h"
#include "TherapyDP.h"
#include "TherapyJson.h"
#include "Dom/JsonObject.h"
#include "Misc/ConfigCacheIni.h"
#include "Serialization/JsonSerializer.h"

namespace
{
    constexpr int32 MaxRetries = 3;
    constexpr float RetryBackoffSeconds = 1.0f;
}

void UTherapyApiClientSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    RefreshConfig();
}

void UTherapyApiClientSubsystem::RefreshConfig()
{
    const FString Section = TEXT("/Script/TherapyDP.TherapyDPSettings");
    GConfig->GetString(*Section, TEXT("BaseUrl"), Config.BaseUrl, GGameIni);
    GConfig->GetString(*Section, TEXT("AiMode"), Config.AiMode, GGameIni);
    GConfig->GetString(*Section, TEXT("OpenAiApiKey"), Config.OpenAiApiKey, GGameIni);
    GConfig->GetString(*Section, TEXT("LocalSttUrl"), Config.LocalSttUrl, GGameIni);
    GConfig->GetString(*Section, TEXT("LocalLlmUrl"), Config.LocalLlmUrl, GGameIni);
    GConfig->GetString(*Section, TEXT("LocalLlmModel"), Config.LocalLlmModel, GGameIni);
    GConfig->GetString(*Section, TEXT("DbPath"), Config.DbPath, GGameIni);
}

void UTherapyApiClientSubsystem::ConfigureRequest(const TSharedRef<IHttpRequest, ESPMode::ThreadSafe>& Request) const
{
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    if (!Config.OpenAiApiKey.IsEmpty())
    {
        Request->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *Config.OpenAiApiKey));
    }
}

void UTherapyApiClientSubsystem::ExecuteWithRetry(const TSharedRef<IHttpRequest, ESPMode::ThreadSafe>& Request, TFunction<void(bool, const FString&)> Completion, int32 RetryCount) const
{
    Request->OnProcessRequestComplete().BindLambda([this, Completion, RetryCount](FHttpRequestPtr Req, FHttpResponsePtr Response, bool bWasSuccessful)
    {
        const bool bResponseOk = bWasSuccessful && Response.IsValid() && EHttpResponseCodes::IsOk(Response->GetResponseCode());
        if (bResponseOk)
        {
            Completion(true, Response->GetContentAsString());
            return;
        }

        if (RetryCount < MaxRetries)
        {
            const float Delay = RetryBackoffSeconds * FMath::Pow(2.0f, RetryCount);
            UE_LOG(LogTherapyDP, Warning, TEXT("HTTP request failed, retrying in %.2fs"), Delay);
            FTimerHandle TimerHandle;
            if (const UWorld* World = GetWorld())
            {
                World->GetTimerManager().SetTimer(TimerHandle, [this, Req, Completion, RetryCount]()
                {
                    ExecuteWithRetry(Req.ToSharedRef(), Completion, RetryCount + 1);
                }, Delay, false);
                return;
            }
        }

        Completion(false, Response.IsValid() ? Response->GetContentAsString() : FString());
    });

    Request->ProcessRequest();
}

void UTherapyApiClientSubsystem::GetExercises(const FTherapyApiExerciseList& Callback)
{
    RefreshConfig();
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(Config.BaseUrl / TEXT("api/v1/exercises"));
    Request->SetVerb(TEXT("GET"));
    ConfigureRequest(Request);

    ExecuteWithRetry(Request, [Callback](bool bSuccess, const FString& Body)
    {
        TArray<FTherapyExercise> Exercises;
        if (bSuccess)
        {
            TSharedPtr<FJsonValue> RootValue;
            const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Body);
            if (FJsonSerializer::Deserialize(Reader, RootValue) && RootValue.IsValid())
            {
                TherapyJson::ParseExerciseArray(RootValue->AsArray(), Exercises);
            }
        }
        Callback.ExecuteIfBound(bSuccess, Exercises);
    });
}

void UTherapyApiClientSubsystem::GetExerciseById(const FString& ExerciseId, const FTherapyApiExerciseDetail& Callback)
{
    RefreshConfig();
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(Config.BaseUrl / TEXT("api/v1/exercises/") + ExerciseId);
    Request->SetVerb(TEXT("GET"));
    ConfigureRequest(Request);

    ExecuteWithRetry(Request, [Callback](bool bSuccess, const FString& Body)
    {
        FTherapyExercise Exercise;
        if (bSuccess)
        {
            TSharedPtr<FJsonObject> RootObj;
            const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Body);
            if (FJsonSerializer::Deserialize(Reader, RootObj))
            {
                TherapyJson::ParseExercise(RootObj, Exercise);
            }
        }
        Callback.ExecuteIfBound(bSuccess, Exercise);
    });
}

void UTherapyApiClientSubsystem::RunPractice(const FString& ExerciseId, const FString& AttemptId, const FString& AudioBase64, const FString& Mode, const FTherapyApiEvaluation& Callback)
{
    RefreshConfig();
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(Config.BaseUrl / TEXT("api/v1/practice/run"));
    Request->SetVerb(TEXT("POST"));
    ConfigureRequest(Request);

    const TSharedPtr<FJsonObject> Payload = TherapyJson::BuildPracticeRunPayload(ExerciseId, AttemptId, AudioBase64, Mode);
    FString Body;
    const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Body);
    FJsonSerializer::Serialize(Payload.ToSharedRef(), Writer);
    Request->SetContentAsString(Body);

    ExecuteWithRetry(Request, [Callback](bool bSuccess, const FString& ResponseBody)
    {
        FTherapyEvaluationResult Result;
        if (bSuccess)
        {
            TSharedPtr<FJsonObject> RootObj;
            const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseBody);
            if (FJsonSerializer::Deserialize(Reader, RootObj))
            {
                TherapyJson::ParseEvaluation(RootObj, Result);
            }
        }
        Callback.ExecuteIfBound(bSuccess, Result);
    });
}
