#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Http.h"
#include "TherapyTypes.h"
#include "TherapyApiClientSubsystem.generated.h"

DECLARE_DELEGATE_TwoParams(FTherapyApiExerciseList, bool, const TArray<FTherapyExercise>&);
DECLARE_DELEGATE_TwoParams(FTherapyApiExerciseDetail, bool, const FTherapyExercise&);
DECLARE_DELEGATE_TwoParams(FTherapyApiEvaluation, bool, const FTherapyEvaluationResult&);

USTRUCT(BlueprintType)
struct FTherapyApiConfig
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString BaseUrl;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString AiMode;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString OpenAiApiKey;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString LocalSttUrl;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString LocalLlmUrl;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString LocalLlmModel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString DbPath;
};

UCLASS()
class THERAPYDP_API UTherapyApiClientSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    UFUNCTION(BlueprintCallable)
    const FTherapyApiConfig& GetConfig() const { return Config; }

    UFUNCTION(BlueprintCallable)
    void RefreshConfig();

    UFUNCTION(BlueprintCallable)
    void GetExercises(const FTherapyApiExerciseList& Callback);

    UFUNCTION(BlueprintCallable)
    void GetExerciseById(const FString& ExerciseId, const FTherapyApiExerciseDetail& Callback);

    UFUNCTION(BlueprintCallable)
    void RunPractice(const FString& ExerciseId, const FString& AttemptId, const FString& AudioBase64, const FString& Mode, const FTherapyApiEvaluation& Callback);

private:
    FTherapyApiConfig Config;

    void ConfigureRequest(const TSharedRef<IHttpRequest, ESPMode::ThreadSafe>& Request) const;
    void ExecuteWithRetry(const TSharedRef<IHttpRequest, ESPMode::ThreadSafe>& Request, TFunction<void(bool, const FString&)> Completion, int32 RetryCount = 0) const;
};
