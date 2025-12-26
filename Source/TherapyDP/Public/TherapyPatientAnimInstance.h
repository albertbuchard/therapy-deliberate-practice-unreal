#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GameplayTagContainer.h"
#include "TherapyPatientAnimInstance.generated.h"

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
    FGameplayTag ActionTag;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Affect")
    float ActionBlend = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Affect")
    float BlinkRate = 0.3f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Affect")
    float GazeArousal = 0.5f;
};
