#include "TherapySessionSubsystem.h"
#include "TherapyDP.h"

void UTherapySessionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    ApiClient = GetGameInstance()->GetSubsystem<UTherapyApiClientSubsystem>();
    TransitionTo(ETherapySessionState::Idle);
}

void UTherapySessionSubsystem::StartRecording()
{
    if (State != ETherapySessionState::Idle && State != ETherapySessionState::Ready)
    {
        return;
    }
    TransitionTo(ETherapySessionState::Recording);
}

void UTherapySessionSubsystem::StopRecording()
{
    if (State != ETherapySessionState::Recording)
    {
        return;
    }
    TransitionTo(ETherapySessionState::Processing);
}

void UTherapySessionSubsystem::SubmitAttempt(const FString& ExerciseId, const FString& AttemptId, const FString& AudioBase64, const FString& Mode)
{
    if (!ApiClient)
    {
        TransitionTo(ETherapySessionState::Error);
        return;
    }

    TransitionTo(ETherapySessionState::Processing);
    ApiClient->RunPractice(ExerciseId, AttemptId, AudioBase64, Mode, FTherapyApiEvaluation::CreateLambda([this](bool bSuccess, const FTherapyEvaluationResult& Result)
    {
        if (!bSuccess)
        {
            TransitionTo(ETherapySessionState::Error);
            return;
        }
        TransitionTo(ETherapySessionState::Ready);
        OnEvaluationReady.Broadcast(Result);
    }));
}

void UTherapySessionSubsystem::TransitionTo(ETherapySessionState NewState)
{
    if (State == NewState)
    {
        return;
    }
    State = NewState;
    OnStateChanged.Broadcast(State);
    UE_LOG(LogTherapyDP, Log, TEXT("Session state -> %d"), static_cast<int32>(State));
}
