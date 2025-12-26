#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "TherapyPatientRegistrySubsystem.generated.h"

class UTherapyPatientDefinition;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTherapyPatientSelectionChanged, UTherapyPatientDefinition*, NewPatient);

UCLASS()
class THERAPYDP_API UTherapyPatientRegistrySubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    UFUNCTION(BlueprintCallable)
    void RefreshRegistry();

    UFUNCTION(BlueprintCallable)
    const TArray<UTherapyPatientDefinition*>& GetPatients() const { return PatientDefinitions; }

    UFUNCTION(BlueprintCallable)
    UTherapyPatientDefinition* GetCurrentPatient() const { return CurrentPatient; }

    UFUNCTION(BlueprintCallable)
    void SetCurrentPatient(UTherapyPatientDefinition* NewPatient);

    UFUNCTION(BlueprintCallable)
    void SetCurrentPatientByIndex(int32 Index);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="TherapyDP")
    TArray<FDirectoryPath> SearchPaths;

    UPROPERTY(BlueprintAssignable)
    FTherapyPatientSelectionChanged OnPatientSelectionChanged;

private:
    void EnsureDefaultSearchPath();

    UPROPERTY()
    TArray<TObjectPtr<UTherapyPatientDefinition>> PatientDefinitions;

    UPROPERTY()
    TObjectPtr<UTherapyPatientDefinition> CurrentPatient;
};
