#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "TherapyApiClientSubsystem.h"
#include "TherapyTypes.h"
#include "TherapySessionSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTherapySessionStateChanged, ETherapySessionState, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTherapySessionEvaluationReady, const FTherapyEvaluationResult&, Result);

UCLASS()
class THERAPYDP_API UTherapySessionSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    UFUNCTION(BlueprintCallable)
    void StartRecording();

    UFUNCTION(BlueprintCallable)
    void StopRecording();

    UFUNCTION(BlueprintCallable)
    void SubmitAttempt(const FString& ExerciseId, const FString& AttemptId, const FString& AudioBase64, const FString& Mode);

    UFUNCTION(BlueprintCallable)
    ETherapySessionState GetState() const { return State; }

    UPROPERTY(BlueprintAssignable)
    FTherapySessionStateChanged OnStateChanged;

    UPROPERTY(BlueprintAssignable)
    FTherapySessionEvaluationReady OnEvaluationReady;

private:
    void TransitionTo(ETherapySessionState NewState);

    ETherapySessionState State = ETherapySessionState::Idle;

    UPROPERTY()
    UTherapyApiClientSubsystem* ApiClient = nullptr;
};
