#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TherapyTypes.h"
#include "TherapyPatientReactionRouter.generated.h"

class UMetaHumanAffectControllerComponent;
class UTherapySessionSubsystem;

UCLASS(BlueprintType, Blueprintable)
class THERAPYDP_API ATherapyPatientReactionRouter : public AActor
{
    GENERATED_BODY()

public:
    ATherapyPatientReactionRouter();

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="TherapyDP")
    TObjectPtr<AActor> PatientActor;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="TherapyDP")
    FName PatientActorTag = TEXT("TherapyPatient");

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="TherapyDP")
    TSubclassOf<AActor> PatientActorClass;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="TherapyDP")
    bool bAutoFindPatient = true;

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
    UFUNCTION()
    void HandleEvaluationReady(const FTherapyEvaluationResult& Result);

    void CachePatientComponent();

    UPROPERTY()
    UTherapySessionSubsystem* SessionSubsystem = nullptr;

    TWeakObjectPtr<UMetaHumanAffectControllerComponent> PatientAffectComponent;
};
