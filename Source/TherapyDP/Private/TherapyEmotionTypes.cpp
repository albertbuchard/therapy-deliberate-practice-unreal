#include "TherapyEmotionTypes.h"
#include "GameplayTagContainer.h"

namespace
{
    FGameplayTag BuildTag(const TCHAR* TagName)
    {
        return FGameplayTag::RequestGameplayTag(FName(TagName), false);
    }

    const TArray<FGameplayTag>& GetOrderedTags()
    {
        static TArray<FGameplayTag> Tags = {
            BuildTag(TEXT("neutral")),
            BuildTag(TEXT("warm")),
            BuildTag(TEXT("sad")),
            BuildTag(TEXT("anxious")),
            BuildTag(TEXT("angry")),
            BuildTag(TEXT("relieved")),
            BuildTag(TEXT("engaged"))
        };
        return Tags;
    }
}

int32 TherapyEmotion::GetEmotionCount()
{
    return static_cast<int32>(ETherapyEmotion::Count);
}

int32 TherapyEmotion::GetEmotionIndex(const FGameplayTag& Tag)
{
    const TArray<FGameplayTag>& Tags = GetOrderedTags();
    for (int32 Index = 0; Index < Tags.Num(); ++Index)
    {
        if (Tags[Index] == Tag)
        {
            return Index;
        }
    }

    return INDEX_NONE;
}

FGameplayTag TherapyEmotion::GetEmotionTag(int32 Index)
{
    const TArray<FGameplayTag>& Tags = GetOrderedTags();
    return Tags.IsValidIndex(Index) ? Tags[Index] : FGameplayTag();
}

void TherapyEmotion::GetAllEmotionTags(TArray<FGameplayTag>& OutTags)
{
    OutTags = GetOrderedTags();
}
