#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GameplayTagContainer.h"
#include "TherapyEmotionTypes.h"
#include "TherapyPatientAnimInstance.generated.h"

class UPoseAsset;
class UAnimSequenceBase;
class UAnimMontage;

UCLASS(BlueprintType)
class THERAPYDP_API UTherapyPatientAnimInstance : public UAnimInstance
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Affect")
    FGameplayTag EmotionTag;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Affect")
    float EmotionBlend = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Affect")
    int32 EmotionIntensity = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Affect")
    TArray<float> EmotionWeights;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Affect")
    FGameplayTag ActionTag;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Affect")
    float ActionBlend = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Affect")
    float BlinkRate = 0.3f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Affect")
    float GazeArousal = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Affect")
    TObjectPtr<UPoseAsset> FacePoseAsset;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Affect")
    TObjectPtr<UPoseAsset> BodyPoseAsset;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Affect")
    TObjectPtr<UAnimSequenceBase> ActionAdditiveSequence;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Affect")
    TObjectPtr<UAnimMontage> ActionMontage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Affect")
    float ActionBlendInTime = 0.1f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Affect")
    float ActionBlendOutTime = 0.2f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Affect")
    float ActionOverlayWeight = 1.0f;

    UFUNCTION(BlueprintCallable, Category = "Affect")
    float GetEmotionWeightByTag(const FGameplayTag& Tag) const;
};
