#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataAsset.h"
#include "TherapyTypes.h"
#include "GameplayTagContainer.h"
#include "TherapyEmotionCurveSolver.h"
#include "MetaHumanAffectControllerComponent.generated.h"

class UPoseAsset;
class UAnimSequenceBase;
class UAnimMontage;
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

USTRUCT(BlueprintType)
struct FTherapyEmotionPoseIntensity
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSoftObjectPtr<UPoseAsset> FacePose;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSoftObjectPtr<UPoseAsset> BodyPose;
};

USTRUCT(BlueprintType)
struct FTherapyEmotionPoseSet
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TArray<FTherapyEmotionPoseIntensity> Intensities;
};

USTRUCT(BlueprintType)
struct FTherapyActionOverlayDefinition
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSoftObjectPtr<UAnimSequenceBase> AdditiveSequence;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSoftObjectPtr<UAnimMontage> Montage;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float BlendInTime = 0.1f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float BlendOutTime = 0.2f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float Weight = 1.0f;
};

USTRUCT(BlueprintType)
struct FTherapyEmotionRuntimeTuning
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float RiseSeconds = 0.35f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float FallSeconds = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MaxChangeRate = 2.0f;
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
    TMap<FGameplayTag, FTherapyEmotionPoseSet> EmotionPoseAssets;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TMap<FGameplayTag, FTherapyActionOverlayDefinition> ActionOverlays;
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

    UFUNCTION(BlueprintCallable)
    void SetSpeedMultiplier(float InMultiplier);

    UFUNCTION(BlueprintCallable)
    void SetPerEmotionTuning(FGameplayTag EmotionTag, float RiseSeconds, float FallSeconds, float MaxChangeRate);

    UFUNCTION(BlueprintCallable)
    TArray<float> GetEmotionWeights() const;

    UFUNCTION(BlueprintCallable)
    FGameplayTag GetDominantEmotionTag() const;

    UFUNCTION(BlueprintCallable)
    FTherapyPatientReaction GetCurrentTargetReaction() const { return CurrentReaction; }

    UFUNCTION(BlueprintCallable)
    bool BindMeshes(USkeletalMeshComponent* InBodyMesh, USkeletalMeshComponent* InFaceMesh);

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UTherapyEmotionMapDataAsset* EmotionMap = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float SpeedMultiplier = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="MetaHuman")
    TObjectPtr<USkeletalMeshComponent> BodyMesh;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="MetaHuman")
    TObjectPtr<USkeletalMeshComponent> FaceMesh;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="MetaHuman")
    FName BodyMeshComponentName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="MetaHuman")
    FName FaceMeshComponentName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="MetaHuman")
    FName BodyMeshComponentTag = TEXT("TherapyDP.BodyMesh");

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="MetaHuman")
    FName FaceMeshComponentTag = TEXT("TherapyDP.FaceMesh");

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
    void UpdateAnimInstance();
    float GetEmotionPriority(const FGameplayTag& Tag) const;
    void CacheAnimInstances();
    USkeletalMeshComponent* ResolveMeshByName(const FName& ComponentName) const;
    USkeletalMeshComponent* ResolveMeshByTag(const FName& ComponentTag) const;
    void ApplyToAnimInstance(UTherapyPatientAnimInstance* AnimInstance) const;
    void UpdateCurveParamsFromMap();
    void UpdateTargetsFromReaction();
    void UpdatePoseAssets();
    void RequestPoseAssetLoad(const TSoftObjectPtr<UPoseAsset>& AssetToLoad, TObjectPtr<UPoseAsset>* Output, TSoftObjectPtr<UPoseAsset>& PendingSlot);
    void RequestActionLoad(const FTherapyActionOverlayDefinition& Overlay);

    UPROPERTY()
    FTherapyPatientReaction CurrentReaction;

    TWeakObjectPtr<UTherapyPatientAnimInstance> BodyAnimInstance;
    TWeakObjectPtr<UTherapyPatientAnimInstance> FaceAnimInstance;

    FTherapyEmotionCurveSolver CurveSolver;

    TMap<FGameplayTag, FTherapyEmotionRuntimeTuning> RuntimeTuningOverrides;

    FGameplayTag ActiveEmotion;
    FGameplayTag ActiveAction;
    float ActionBlend = 0.0f;
    bool bListening = false;

    UPROPERTY()
    TObjectPtr<UTherapyEmotionMapDataAsset> CachedEmotionMap;

    FGameplayTag LastPoseEmotion;
    int32 LastPoseIntensity = INDEX_NONE;
    FGameplayTag LastPoseAction;

    UPROPERTY()
    TObjectPtr<UPoseAsset> CachedFacePoseAsset;

    UPROPERTY()
    TObjectPtr<UPoseAsset> CachedBodyPoseAsset;

    UPROPERTY()
    TObjectPtr<UAnimSequenceBase> CachedActionSequence;

    UPROPERTY()
    TObjectPtr<UAnimMontage> CachedActionMontage;

    TSoftObjectPtr<UPoseAsset> PendingFacePose;
    TSoftObjectPtr<UPoseAsset> PendingBodyPose;
    TSoftObjectPtr<UAnimSequenceBase> PendingActionSequence;
    TSoftObjectPtr<UAnimMontage> PendingActionMontage;
};
