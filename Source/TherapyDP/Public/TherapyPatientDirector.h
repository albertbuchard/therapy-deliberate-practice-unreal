#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TherapyTypes.h"
#include "TherapyPatientDirector.generated.h"

class UMetaHumanAffectControllerComponent;
class UTherapySessionSubsystem;
class UTherapyPatientRegistrySubsystem;
class UTherapyPatientDefinition;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTherapyActiveAffectControllerChanged, UMetaHumanAffectControllerComponent*, NewController);

UCLASS(BlueprintType, Blueprintable)
class THERAPYDP_API ATherapyPatientDirector : public AActor
{
    GENERATED_BODY()

public:
    ATherapyPatientDirector();

    UFUNCTION(BlueprintCallable)
    UMetaHumanAffectControllerComponent* GetActiveAffectController() const { return PatientAffectComponent.Get(); }

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="TherapyDP")
    TObjectPtr<AActor> PatientActorOverride;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="TherapyDP")
    bool bAutoSpawnSelectedPatient = true;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="TherapyDP")
    bool bDestroyPreviousPatientOnSelection = true;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="TherapyDP")
    FTransform PatientSpawnTransform;

    UPROPERTY(BlueprintAssignable, Category="TherapyDP")
    FTherapyActiveAffectControllerChanged OnActiveAffectControllerChanged;

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
    UFUNCTION()
    void HandleEvaluationReady(const FTherapyEvaluationResult& Result);

    UFUNCTION()
    void HandleSessionStateChanged(ETherapySessionState NewState);

    UFUNCTION()
    void HandlePatientSelectionChanged(UTherapyPatientDefinition* NewPatient);

    void RefreshPatientActor();
    void CachePatientComponent();

    UPROPERTY()
    UTherapySessionSubsystem* SessionSubsystem = nullptr;

    UPROPERTY()
    UTherapyPatientRegistrySubsystem* PatientRegistry = nullptr;

    UPROPERTY()
    TObjectPtr<AActor> ActivePatientActor;

    TWeakObjectPtr<UMetaHumanAffectControllerComponent> PatientAffectComponent;
};
