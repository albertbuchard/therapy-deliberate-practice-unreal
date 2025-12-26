#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "TherapyPatientDefinition.generated.h"

class UTherapyEmotionMapDataAsset;

UCLASS(BlueprintType)
class THERAPYDP_API UTherapyPatientDefinition : public UDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSoftClassPtr<AActor> PatientActorClass;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TObjectPtr<UTherapyEmotionMapDataAsset> EmotionMap;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FVector CameraOffset = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FRotator CameraRotation = FRotator::ZeroRotator;
};
