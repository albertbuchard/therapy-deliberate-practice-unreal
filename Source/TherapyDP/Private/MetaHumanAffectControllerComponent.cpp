#include "MetaHumanAffectControllerComponent.h"
#include "TherapyEmotionTypes.h"
#include "TherapyPatientAnimInstance.h"
#include "AssetManager.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/StreamableManager.h"
#include "GameFramework/Actor.h"

UMetaHumanAffectControllerComponent::UMetaHumanAffectControllerComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UMetaHumanAffectControllerComponent::BeginPlay()
{
    Super::BeginPlay();

    CurveSolver.Initialize(TherapyEmotion::GetEmotionCount());
    CurveSolver.SetGlobalSpeedMultiplier(SpeedMultiplier);
    UpdateCurveParamsFromMap();
    CacheAnimInstances();
    UpdateTargetsFromReaction();
}

void UMetaHumanAffectControllerComponent::ApplyReaction(const FTherapyPatientReaction& Reaction)
{
    CurrentReaction = Reaction;

    if (bListening)
    {
        return;
    }

    const float CurrentPriority = GetEmotionPriority(ActiveEmotion);
    const float NewPriority = GetEmotionPriority(Reaction.EmotionTag);

    if (NewPriority >= CurrentPriority || !ActiveEmotion.IsValid())
    {
        ActiveEmotion = Reaction.EmotionTag;
        ActiveAction = Reaction.ActionTag;
        ActionBlend = 0.0f;
        UpdateTargetsFromReaction();
        UpdatePoseAssets();
    }
}

void UMetaHumanAffectControllerComponent::SetListening(bool bInListening)
{
    bListening = bInListening;
    if (bListening)
    {
        ActiveEmotion = FGameplayTag::RequestGameplayTag(TEXT("neutral"), false);
        ActiveAction = FGameplayTag();
        CurrentReaction.EmotionTag = ActiveEmotion;
        CurrentReaction.Intensity = 1;
        CurrentReaction.ActionTag = FGameplayTag();
    }

    UpdateTargetsFromReaction();
    UpdatePoseAssets();
}

void UMetaHumanAffectControllerComponent::SetSpeedMultiplier(float InMultiplier)
{
    SpeedMultiplier = FMath::Clamp(InMultiplier, 0.25f, 3.0f);
    CurveSolver.SetGlobalSpeedMultiplier(SpeedMultiplier);
}

void UMetaHumanAffectControllerComponent::SetPerEmotionTuning(FGameplayTag EmotionTag, float RiseSeconds, float FallSeconds, float MaxChangeRate)
{
    FTherapyEmotionRuntimeTuning& Tuning = RuntimeTuningOverrides.FindOrAdd(EmotionTag);
    Tuning.RiseSeconds = RiseSeconds;
    Tuning.FallSeconds = FallSeconds;
    Tuning.MaxChangeRate = MaxChangeRate;

    UpdateCurveParamsFromMap();
}

TArray<float> UMetaHumanAffectControllerComponent::GetEmotionWeights() const
{
    TArray<float> Weights = CurveSolver.GetWeights();
    float WeightSum = 0.0f;
    for (const float Weight : Weights)
    {
        WeightSum += Weight;
    }

    if (WeightSum > 1.0f)
    {
        const float NormalizeFactor = 1.0f / WeightSum;
        for (float& Weight : Weights)
        {
            Weight *= NormalizeFactor;
        }
    }

    return Weights;
}

FGameplayTag UMetaHumanAffectControllerComponent::GetDominantEmotionTag() const
{
    const TArray<float>& Weights = CurveSolver.GetWeights();
    int32 DominantIndex = INDEX_NONE;
    float BestWeight = 0.0f;
    for (int32 Index = 0; Index < Weights.Num(); ++Index)
    {
        if (Weights[Index] > BestWeight)
        {
            BestWeight = Weights[Index];
            DominantIndex = Index;
        }
    }

    return DominantIndex != INDEX_NONE ? TherapyEmotion::GetEmotionTag(DominantIndex) : FGameplayTag();
}

bool UMetaHumanAffectControllerComponent::BindMeshes(USkeletalMeshComponent* InBodyMesh, USkeletalMeshComponent* InFaceMesh)
{
    BodyMesh = InBodyMesh;
    FaceMesh = InFaceMesh;
    CacheAnimInstances();

    return BodyMesh != nullptr || FaceMesh != nullptr;
}

void UMetaHumanAffectControllerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (EmotionMap != CachedEmotionMap)
    {
        CachedEmotionMap = EmotionMap;
        UpdateCurveParamsFromMap();
        UpdateTargetsFromReaction();
        UpdatePoseAssets();
    }

    if (!BodyAnimInstance.IsValid() && !FaceAnimInstance.IsValid())
    {
        CacheAnimInstances();
    }

    if (bListening)
    {
        ActiveEmotion = FGameplayTag::RequestGameplayTag(TEXT("neutral"), false);
    }

    CurveSolver.Tick(DeltaTime);

    const float ActionTarget = ActiveAction.IsValid() ? 1.0f : 0.0f;
    ActionBlend = FMath::FInterpTo(ActionBlend, ActionTarget, DeltaTime, 4.0f * SpeedMultiplier);

    UpdateAnimInstance();
}

void UMetaHumanAffectControllerComponent::UpdateAnimInstance()
{
    if (BodyAnimInstance.IsValid())
    {
        ApplyToAnimInstance(BodyAnimInstance.Get());
    }

    if (FaceAnimInstance.IsValid() && FaceAnimInstance.Get() != BodyAnimInstance.Get())
    {
        ApplyToAnimInstance(FaceAnimInstance.Get());
    }
}

float UMetaHumanAffectControllerComponent::GetEmotionPriority(const FGameplayTag& Tag) const
{
    if (!EmotionMap)
    {
        return 0.0f;
    }

    if (const int32* Priority = EmotionMap->EmotionPriority.Find(Tag))
    {
        return static_cast<float>(*Priority);
    }

    return 0.0f;
}

void UMetaHumanAffectControllerComponent::CacheAnimInstances()
{
    AActor* Owner = GetOwner();
    if (!Owner)
    {
        return;
    }

    if (!BodyMesh)
    {
        BodyMesh = ResolveMeshByName(BodyMeshComponentName);
    }

    if (!FaceMesh)
    {
        FaceMesh = ResolveMeshByName(FaceMeshComponentName);
    }

    if (!BodyMesh)
    {
        BodyMesh = ResolveMeshByTag(BodyMeshComponentTag);
    }

    if (!FaceMesh)
    {
        FaceMesh = ResolveMeshByTag(FaceMeshComponentTag);
    }

    if (!BodyMesh || !FaceMesh)
    {
        TArray<USkeletalMeshComponent*> Meshes;
        Owner->GetComponents(Meshes);
        for (USkeletalMeshComponent* Mesh : Meshes)
        {
            if (!Mesh)
            {
                continue;
            }

            if (!BodyMesh && Mesh->ComponentHasTag(BodyMeshComponentTag))
            {
                BodyMesh = Mesh;
                continue;
            }

            if (!FaceMesh && Mesh->ComponentHasTag(FaceMeshComponentTag))
            {
                FaceMesh = Mesh;
            }
        }

        if (!BodyMesh && Meshes.Num() > 0)
        {
            BodyMesh = Meshes[0];
        }

        if (!FaceMesh && Meshes.Num() > 1)
        {
            FaceMesh = Meshes[1];
        }
    }

    BodyAnimInstance = BodyMesh ? Cast<UTherapyPatientAnimInstance>(BodyMesh->GetAnimInstance()) : nullptr;
    FaceAnimInstance = FaceMesh ? Cast<UTherapyPatientAnimInstance>(FaceMesh->GetAnimInstance()) : nullptr;
}

USkeletalMeshComponent* UMetaHumanAffectControllerComponent::ResolveMeshByName(const FName& ComponentName) const
{
    if (ComponentName.IsNone())
    {
        return nullptr;
    }

    AActor* Owner = GetOwner();
    if (!Owner)
    {
        return nullptr;
    }

    TArray<USkeletalMeshComponent*> Meshes;
    Owner->GetComponents(Meshes);
    for (USkeletalMeshComponent* Mesh : Meshes)
    {
        if (Mesh && Mesh->GetFName() == ComponentName)
        {
            return Mesh;
        }
    }

    return nullptr;
}

USkeletalMeshComponent* UMetaHumanAffectControllerComponent::ResolveMeshByTag(const FName& ComponentTag) const
{
    if (ComponentTag.IsNone())
    {
        return nullptr;
    }

    AActor* Owner = GetOwner();
    if (!Owner)
    {
        return nullptr;
    }

    TArray<USkeletalMeshComponent*> Meshes;
    Owner->GetComponents(Meshes);
    for (USkeletalMeshComponent* Mesh : Meshes)
    {
        if (Mesh && Mesh->ComponentHasTag(ComponentTag))
        {
            return Mesh;
        }
    }

    return nullptr;
}

void UMetaHumanAffectControllerComponent::ApplyToAnimInstance(UTherapyPatientAnimInstance* AnimInstance) const
{
    if (!AnimInstance)
    {
        return;
    }

    const TArray<float>& Weights = CurveSolver.GetWeights();
    TArray<float> NormalizedWeights = Weights;
    float WeightSum = 0.0f;
    for (const float Weight : NormalizedWeights)
    {
        WeightSum += Weight;
    }

    if (WeightSum > 1.0f)
    {
        const float NormalizeFactor = 1.0f / WeightSum;
        for (float& Weight : NormalizedWeights)
        {
            Weight *= NormalizeFactor;
        }
    }

    AnimInstance->EmotionWeights = NormalizedWeights;
    AnimInstance->EmotionTag = GetDominantEmotionTag();
    const int32 DominantIndex = TherapyEmotion::GetEmotionIndex(AnimInstance->EmotionTag);
    AnimInstance->EmotionBlend = NormalizedWeights.IsValidIndex(DominantIndex) ? FMath::Clamp(NormalizedWeights[DominantIndex], 0.0f, 1.0f) : 0.0f;
    AnimInstance->EmotionIntensity = CurrentReaction.Intensity;
    AnimInstance->ActionTag = ActiveAction;
    AnimInstance->ActionBlend = ActionBlend;

    const FTherapyEmotionTuning* Tuning = nullptr;
    if (EmotionMap && AnimInstance->EmotionTag.IsValid())
    {
        Tuning = EmotionMap->EmotionTuning.Find(AnimInstance->EmotionTag);
    }

    AnimInstance->BlinkRate = Tuning ? Tuning->BlinkRate : AnimInstance->BlinkRate;
    AnimInstance->GazeArousal = Tuning ? Tuning->GazeArousal : AnimInstance->GazeArousal;
    AnimInstance->FacePoseAsset = CachedFacePoseAsset;
    AnimInstance->BodyPoseAsset = CachedBodyPoseAsset;
    AnimInstance->ActionAdditiveSequence = CachedActionSequence;
    AnimInstance->ActionMontage = CachedActionMontage;

    if (EmotionMap && ActiveAction.IsValid())
    {
        if (const FTherapyActionOverlayDefinition* Overlay = EmotionMap->ActionOverlays.Find(ActiveAction))
        {
            AnimInstance->ActionBlendInTime = Overlay->BlendInTime;
            AnimInstance->ActionBlendOutTime = Overlay->BlendOutTime;
            AnimInstance->ActionOverlayWeight = Overlay->Weight;
            return;
        }
    }

    AnimInstance->ActionBlendInTime = 0.1f;
    AnimInstance->ActionBlendOutTime = 0.2f;
    AnimInstance->ActionOverlayWeight = 1.0f;
}

void UMetaHumanAffectControllerComponent::UpdateCurveParamsFromMap()
{
    const int32 EmotionCount = TherapyEmotion::GetEmotionCount();

    for (int32 Index = 0; Index < EmotionCount; ++Index)
    {
        FTherapyEmotionCurveParams Params;
        const FGameplayTag Tag = TherapyEmotion::GetEmotionTag(Index);

        if (EmotionMap)
        {
            if (const FTherapyEmotionTuning* Tuning = EmotionMap->EmotionTuning.Find(Tag))
            {
                Params.RiseSeconds = Tuning->OnsetSeconds;
                Params.FallSeconds = Tuning->DecaySeconds;
                Params.HoldSeconds = Tuning->HoldSeconds;
            }
        }

        if (const FTherapyEmotionRuntimeTuning* Override = RuntimeTuningOverrides.Find(Tag))
        {
            Params.RiseSeconds = Override->RiseSeconds;
            Params.FallSeconds = Override->FallSeconds;
            Params.MaxChangeRate = Override->MaxChangeRate;
        }

        CurveSolver.SetParams(Index, Params);
    }
}

void UMetaHumanAffectControllerComponent::UpdateTargetsFromReaction()
{
    const int32 EmotionCount = TherapyEmotion::GetEmotionCount();
    for (int32 Index = 0; Index < EmotionCount; ++Index)
    {
        CurveSolver.SetTargetWeight(Index, 0.0f);
    }

    if (bListening)
    {
        const int32 NeutralIndex = TherapyEmotion::GetEmotionIndex(FGameplayTag::RequestGameplayTag(TEXT("neutral"), false));
        if (NeutralIndex != INDEX_NONE)
        {
            CurveSolver.SetTarget(NeutralIndex, 1.0f);
        }
        return;
    }

    if (!ActiveEmotion.IsValid())
    {
        return;
    }

    const int32 ActiveIndex = TherapyEmotion::GetEmotionIndex(ActiveEmotion);
    if (ActiveIndex == INDEX_NONE)
    {
        return;
    }

    const float TargetWeight = FMath::Clamp(static_cast<float>(CurrentReaction.Intensity) / 3.0f, 0.0f, 1.0f);
    float HoldSeconds = -1.0f;
    if (EmotionMap)
    {
        if (const FTherapyEmotionTuning* Tuning = EmotionMap->EmotionTuning.Find(ActiveEmotion))
        {
            HoldSeconds = Tuning->HoldSeconds;
        }
    }

    CurveSolver.SetTarget(ActiveIndex, TargetWeight, HoldSeconds);
}

void UMetaHumanAffectControllerComponent::UpdatePoseAssets()
{
    if (!EmotionMap)
    {
        CachedFacePoseAsset = nullptr;
        CachedBodyPoseAsset = nullptr;
        CachedActionSequence = nullptr;
        CachedActionMontage = nullptr;
        return;
    }

    const int32 IntensityIndex = FMath::Clamp(CurrentReaction.Intensity, 0, 3);

    if (ActiveEmotion != LastPoseEmotion || IntensityIndex != LastPoseIntensity)
    {
        LastPoseEmotion = ActiveEmotion;
        LastPoseIntensity = IntensityIndex;

        if (const FTherapyEmotionPoseSet* PoseSet = EmotionMap->EmotionPoseAssets.Find(ActiveEmotion))
        {
            if (PoseSet->Intensities.IsValidIndex(IntensityIndex))
            {
                const FTherapyEmotionPoseIntensity& PoseIntensity = PoseSet->Intensities[IntensityIndex];
                RequestPoseAssetLoad(PoseIntensity.FacePose, &CachedFacePoseAsset, PendingFacePose);
                RequestPoseAssetLoad(PoseIntensity.BodyPose, &CachedBodyPoseAsset, PendingBodyPose);
            }
            else
            {
                CachedFacePoseAsset = nullptr;
                CachedBodyPoseAsset = nullptr;
            }
        }
        else
        {
            CachedFacePoseAsset = nullptr;
            CachedBodyPoseAsset = nullptr;
        }
    }

    if (ActiveAction != LastPoseAction)
    {
        LastPoseAction = ActiveAction;
        if (const FTherapyActionOverlayDefinition* Overlay = EmotionMap->ActionOverlays.Find(ActiveAction))
        {
            RequestActionLoad(*Overlay);
        }
        else
        {
            CachedActionSequence = nullptr;
            CachedActionMontage = nullptr;
            PendingActionSequence.Reset();
            PendingActionMontage.Reset();
        }
    }
}

void UMetaHumanAffectControllerComponent::RequestPoseAssetLoad(const TSoftObjectPtr<UPoseAsset>& AssetToLoad, TObjectPtr<UPoseAsset>* Output, TSoftObjectPtr<UPoseAsset>& PendingSlot)
{
    if (AssetToLoad.IsNull())
    {
        if (Output)
        {
            *Output = nullptr;
        }
        PendingSlot.Reset();
        return;
    }

    if (AssetToLoad.IsValid())
    {
        if (Output)
        {
            *Output = AssetToLoad.Get();
        }
        PendingSlot.Reset();
        return;
    }

    PendingSlot = AssetToLoad;
    FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
    Streamable.RequestAsyncLoad(AssetToLoad.ToSoftObjectPath(), FStreamableDelegate::CreateWeakLambda(this, [this, AssetToLoad, Output, &PendingSlot]()
    {
        if (PendingSlot == AssetToLoad)
        {
            if (Output)
            {
                *Output = AssetToLoad.Get();
            }
            PendingSlot.Reset();
        }
    }));
}

void UMetaHumanAffectControllerComponent::RequestActionLoad(const FTherapyActionOverlayDefinition& Overlay)
{
    if (Overlay.AdditiveSequence.IsNull())
    {
        CachedActionSequence = nullptr;
    }
    else if (Overlay.AdditiveSequence.IsValid())
    {
        CachedActionSequence = Overlay.AdditiveSequence.Get();
    }
    else
    {
        PendingActionSequence = Overlay.AdditiveSequence;
        FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
        Streamable.RequestAsyncLoad(Overlay.AdditiveSequence.ToSoftObjectPath(), FStreamableDelegate::CreateWeakLambda(this, [this, Overlay]()
        {
            if (PendingActionSequence == Overlay.AdditiveSequence)
            {
                CachedActionSequence = Overlay.AdditiveSequence.Get();
                PendingActionSequence.Reset();
            }
        }));
    }

    if (Overlay.Montage.IsNull())
    {
        CachedActionMontage = nullptr;
    }
    else if (Overlay.Montage.IsValid())
    {
        CachedActionMontage = Overlay.Montage.Get();
    }
    else
    {
        PendingActionMontage = Overlay.Montage;
        FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
        Streamable.RequestAsyncLoad(Overlay.Montage.ToSoftObjectPath(), FStreamableDelegate::CreateWeakLambda(this, [this, Overlay]()
        {
            if (PendingActionMontage == Overlay.Montage)
            {
                CachedActionMontage = Overlay.Montage.Get();
                PendingActionMontage.Reset();
            }
        }));
    }
}
