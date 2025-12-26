#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "TherapyEmotionTypes.generated.h"

UENUM(BlueprintType)
enum class ETherapyEmotion : uint8
{
    Neutral,
    Warm,
    Sad,
    Anxious,
    Angry,
    Relieved,
    Engaged,
    Count UMETA(Hidden)
};

namespace TherapyEmotion
{
    THERAPYDP_API int32 GetEmotionCount();
    THERAPYDP_API int32 GetEmotionIndex(const FGameplayTag& Tag);
    THERAPYDP_API FGameplayTag GetEmotionTag(int32 Index);
    THERAPYDP_API void GetAllEmotionTags(TArray<FGameplayTag>& OutTags);
}
