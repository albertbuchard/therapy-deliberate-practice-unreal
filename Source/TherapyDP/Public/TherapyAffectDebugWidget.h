#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "TimerManager.h"
#include "TherapyTypes.h"
#include "TherapyAffectDebugWidget.generated.h"

class ATherapyPatientDirector;
class UTherapyPatientDefinition;
class UTherapyPatientRegistrySubsystem;
class UMetaHumanAffectControllerComponent;

UENUM(BlueprintType)
enum class ETherapyAffectPacingPreset : uint8
{
    HumanSlow UMETA(DisplayName="Human Slow"),
    Default UMETA(DisplayName="Default"),
    Fast UMETA(DisplayName="Fast")
};

UCLASS(BlueprintType, Blueprintable)
class THERAPYDP_API UTherapyAffectDebugWidget : public UCommonActivatableWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    void SetAffectController(UMetaHumanAffectControllerComponent* InController);

    UFUNCTION(BlueprintCallable)
    void ApplyGlobalSpeed(float InMultiplier);

    UFUNCTION(BlueprintCallable)
    void ApplyPerEmotionSpeed(const FGameplayTag& EmotionTag, float RiseSeconds, float FallSeconds, float MaxChangeRate);

    UFUNCTION(BlueprintCallable)
    void ApplyPacingPreset(ETherapyAffectPacingPreset Preset);

    UFUNCTION(BlueprintCallable)
    void ClearRuntimeTuningOverrides();

    UFUNCTION(BlueprintCallable)
    FTherapyPatientReaction GetCurrentTargetReaction() const;

    UFUNCTION(BlueprintCallable)
    TArray<float> GetCurrentWeights() const;

    UFUNCTION(BlueprintCallable)
    TArray<FGameplayTag> GetOrderedEmotionTags() const;

    UFUNCTION(BlueprintCallable)
    TArray<FText> GetPatientDisplayNames() const;

    UFUNCTION(BlueprintCallable)
    int32 GetSelectedPatientIndex() const;

    UFUNCTION(BlueprintCallable)
    void SetSelectedPatientIndex(int32 Index);

    UFUNCTION(BlueprintCallable)
    void RefreshPatientRegistry();

    UFUNCTION(BlueprintCallable)
    float GetCurrentSpeedMultiplier() const;

protected:
    virtual void NativeOnInitialized() override;
    virtual void NativeOnActivated() override;
    virtual void NativeOnDeactivated() override;

private:
    void BindToPatientDirector();
    void HandleActiveAffectControllerChanged(UMetaHumanAffectControllerComponent* NewController);
    void HandlePatientSelectionChanged(UTherapyPatientDefinition* NewPatient);
    void RefreshCachedAffectData();
    void CachePatientList();

    UPROPERTY()
    TObjectPtr<UMetaHumanAffectControllerComponent> AffectController;

    UPROPERTY()
    TObjectPtr<UTherapyPatientRegistrySubsystem> PatientRegistry;

    UPROPERTY()
    TObjectPtr<ATherapyPatientDirector> PatientDirector;

    UPROPERTY()
    FTherapyPatientReaction CachedReaction;

    UPROPERTY()
    TArray<float> CachedWeights;

    UPROPERTY()
    TArray<FGameplayTag> OrderedEmotionTags;

    UPROPERTY()
    TArray<FText> CachedPatientNames;

    UPROPERTY()
    int32 CachedSelectedIndex = INDEX_NONE;

    UPROPERTY()
    float RefreshRateSeconds = 0.1f;

    FTimerHandle RefreshTimerHandle;
};
