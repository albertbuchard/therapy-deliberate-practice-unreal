#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "TherapyTypes.h"
#include "TherapyAffectDebugWidget.generated.h"

class UMetaHumanAffectControllerComponent;

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
    FTherapyPatientReaction GetCurrentTargetReaction() const;

    UFUNCTION(BlueprintCallable)
    TArray<float> GetCurrentWeights() const;

    UFUNCTION(BlueprintCallable)
    TArray<FGameplayTag> GetOrderedEmotionTags() const;

private:
    UPROPERTY()
    TObjectPtr<UMetaHumanAffectControllerComponent> AffectController;
};
