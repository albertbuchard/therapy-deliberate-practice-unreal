#include "TherapyAffectDebugWidget.h"
#include "MetaHumanAffectControllerComponent.h"
#include "TherapyEmotionTypes.h"

void UTherapyAffectDebugWidget::SetAffectController(UMetaHumanAffectControllerComponent* InController)
{
    AffectController = InController;
}

void UTherapyAffectDebugWidget::ApplyGlobalSpeed(float InMultiplier)
{
    if (AffectController)
    {
        AffectController->SetSpeedMultiplier(InMultiplier);
    }
}

void UTherapyAffectDebugWidget::ApplyPerEmotionSpeed(const FGameplayTag& EmotionTag, float RiseSeconds, float FallSeconds, float MaxChangeRate)
{
    if (AffectController)
    {
        AffectController->SetPerEmotionTuning(EmotionTag, RiseSeconds, FallSeconds, MaxChangeRate);
    }
}

FTherapyPatientReaction UTherapyAffectDebugWidget::GetCurrentTargetReaction() const
{
    return AffectController ? AffectController->GetCurrentTargetReaction() : FTherapyPatientReaction();
}

TArray<float> UTherapyAffectDebugWidget::GetCurrentWeights() const
{
    return AffectController ? AffectController->GetEmotionWeights() : TArray<float>();
}

TArray<FGameplayTag> UTherapyAffectDebugWidget::GetOrderedEmotionTags() const
{
    TArray<FGameplayTag> Tags;
    TherapyEmotion::GetAllEmotionTags(Tags);
    return Tags;
}
