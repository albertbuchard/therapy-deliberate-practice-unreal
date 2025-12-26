#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataAsset.h"
#include "TherapyTypes.h"
#include "GameplayTagContainer.h"
#include "MetaHumanAffectControllerComponent.generated.h"

class UPoseAsset;
class UAnimSequenceBase;
class USkeletalMeshComponent;
class UTherapyPatientAnimInstance;

USTRUCT(BlueprintType)
struct FTherapyEmotionTuning
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float OnsetSeconds = 0.35f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float HoldSeconds = 1.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float DecaySeconds = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float BlinkRate = 0.3f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float GazeArousal = 0.5f;
};

UCLASS(BlueprintType)
class THERAPYDP_API UTherapyEmotionMapDataAsset : public UDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TMap<FGameplayTag, FTherapyEmotionTuning> EmotionTuning;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TMap<FGameplayTag, int32> EmotionPriority;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TMap<FGameplayTag, FGameplayTag> ActionToOverlay;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TMap<FGameplayTag, TSoftObjectPtr<UPoseAsset>> EmotionPoseAssets;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TMap<FGameplayTag, TSoftObjectPtr<UAnimSequenceBase>> ActionAdditiveSequences;
};

UCLASS(ClassGroup=(TherapyDP), meta=(BlueprintSpawnableComponent))
class THERAPYDP_API UMetaHumanAffectControllerComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UMetaHumanAffectControllerComponent();

    UFUNCTION(BlueprintCallable)
    void ApplyReaction(const FTherapyPatientReaction& Reaction);

    UFUNCTION(BlueprintCallable)
    void SetListening(bool bListening);

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UTherapyEmotionMapDataAsset* EmotionMap;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float GlobalBlendSpeed = 2.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="MetaHuman")
    TObjectPtr<USkeletalMeshComponent> BodyMesh;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="MetaHuman")
    TObjectPtr<USkeletalMeshComponent> FaceMesh;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="MetaHuman")
    FName BodyMeshComponentName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="MetaHuman")
    FName FaceMeshComponentName;

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
    void UpdateAnimInstance(float DeltaTime);
    float GetEmotionPriority(const FGameplayTag& Tag) const;
    void CacheAnimInstances();
    USkeletalMeshComponent* ResolveMeshByName(const FName& ComponentName) const;
    void ApplyToAnimInstance(UTherapyPatientAnimInstance* AnimInstance) const;

    UPROPERTY()
    FTherapyPatientReaction CurrentReaction;

    TWeakObjectPtr<UTherapyPatientAnimInstance> BodyAnimInstance;
    TWeakObjectPtr<UTherapyPatientAnimInstance> FaceAnimInstance;

    FGameplayTag ActiveEmotion;
    FGameplayTag ActiveAction;
    float EmotionBlend = 0.0f;
    float ActionBlend = 0.0f;
    float HoldRemaining = 0.0f;
    bool bListening = false;
};
