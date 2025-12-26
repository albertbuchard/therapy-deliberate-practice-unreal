#include "MetaHumanAffectControllerComponent.h"
#include "TherapyPatientAnimInstance.h"
#include "TherapyDP.h"
#include "GameFramework/Actor.h"
#include "Components/SkeletalMeshComponent.h"

UMetaHumanAffectControllerComponent::UMetaHumanAffectControllerComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UMetaHumanAffectControllerComponent::BeginPlay()
{
    Super::BeginPlay();
    CacheAnimInstances();
}

void UMetaHumanAffectControllerComponent::ApplyReaction(const FTherapyPatientReaction& Reaction)
{
    CurrentReaction = Reaction;

    const float CurrentPriority = GetEmotionPriority(ActiveEmotion);
    const float NewPriority = GetEmotionPriority(Reaction.EmotionTag);
    if (NewPriority >= CurrentPriority)
    {
        ActiveEmotion = Reaction.EmotionTag;
        ActiveAction = Reaction.ActionTag;
        EmotionBlend = 0.0f;
        ActionBlend = 0.0f;
        HoldRemaining = EmotionMap && EmotionMap->EmotionTuning.Contains(ActiveEmotion)
            ? EmotionMap->EmotionTuning[ActiveEmotion].HoldSeconds
            : 1.0f;
    }
}

void UMetaHumanAffectControllerComponent::SetListening(bool bInListening)
{
    bListening = bInListening;
}

void UMetaHumanAffectControllerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!BodyAnimInstance.IsValid() && !FaceAnimInstance.IsValid())
    {
        CacheAnimInstances();
    }

    if (bListening)
    {
        ActiveEmotion = FGameplayTag::RequestGameplayTag(TEXT("neutral"), false);
        ActiveAction = FGameplayTag();
        HoldRemaining = 0.0f;
    }

    if (HoldRemaining > 0.0f)
    {
        HoldRemaining -= DeltaTime;
        EmotionBlend = FMath::Min(1.0f, EmotionBlend + DeltaTime * GlobalBlendSpeed);
    }
    else
    {
        EmotionBlend = FMath::Max(0.0f, EmotionBlend - DeltaTime * GlobalBlendSpeed);
        ActionBlend = FMath::Max(0.0f, ActionBlend - DeltaTime * GlobalBlendSpeed);
    }

    ActionBlend = FMath::Min(1.0f, ActionBlend + DeltaTime * GlobalBlendSpeed);

    UpdateAnimInstance(DeltaTime);
}

void UMetaHumanAffectControllerComponent::UpdateAnimInstance(float DeltaTime)
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

    TArray<USkeletalMeshComponent*> Meshes;
    Owner->GetComponents(Meshes);

    for (USkeletalMeshComponent* Mesh : Meshes)
    {
        if (!Mesh)
        {
            continue;
        }

        const FString MeshName = Mesh->GetName();
        if (!BodyMesh && MeshName.Contains(TEXT("Body"), ESearchCase::IgnoreCase))
        {
            BodyMesh = Mesh;
        }
        if (!FaceMesh && MeshName.Contains(TEXT("Face"), ESearchCase::IgnoreCase))
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

void UMetaHumanAffectControllerComponent::ApplyToAnimInstance(UTherapyPatientAnimInstance* AnimInstance) const
{
    if (!AnimInstance)
    {
        return;
    }

    const FTherapyEmotionTuning* Tuning = EmotionMap && EmotionMap->EmotionTuning.Contains(ActiveEmotion)
        ? &EmotionMap->EmotionTuning[ActiveEmotion]
        : nullptr;

    AnimInstance->EmotionTag = ActiveEmotion;
    AnimInstance->EmotionIntensity = CurrentReaction.Intensity;
    AnimInstance->EmotionBlend = EmotionBlend;
    AnimInstance->ActionTag = ActiveAction;
    AnimInstance->ActionBlend = ActionBlend;
    AnimInstance->BlinkRate = Tuning ? Tuning->BlinkRate : AnimInstance->BlinkRate;
    AnimInstance->GazeArousal = Tuning ? Tuning->GazeArousal : AnimInstance->GazeArousal;
}
