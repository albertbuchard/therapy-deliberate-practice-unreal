#include "TherapyPatientAnimInstance.h"

float UTherapyPatientAnimInstance::GetEmotionWeightByTag(const FGameplayTag& Tag) const
{
    const int32 Index = TherapyEmotion::GetEmotionIndex(Tag);
    return EmotionWeights.IsValidIndex(Index) ? EmotionWeights[Index] : 0.0f;
}
