#include "TherapyCameraControllerComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Actor.h"
#include "Components/SceneComponent.h"

UTherapyCameraControllerComponent::UTherapyCameraControllerComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UTherapyCameraControllerComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UTherapyCameraControllerComponent::ApplyReaction(const FTherapyPatientReaction& Reaction)
{
    if (Reaction.EmotionTag.MatchesTagExact(FGameplayTag::RequestGameplayTag(TEXT("engaged"), false)))
    {
        TargetOffset = -6.0f;
    }
    else if (Reaction.EmotionTag.MatchesTagExact(FGameplayTag::RequestGameplayTag(TEXT("angry"), false)))
    {
        TargetOffset = 6.0f;
    }
    else
    {
        TargetOffset = 0.0f;
    }
}

void UTherapyCameraControllerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    AActor* Owner = GetOwner();
    if (!Owner)
    {
        return;
    }

    USceneComponent* Root = Owner->GetRootComponent();
    if (!Root)
    {
        return;
    }

    CurrentOffset = FMath::FInterpTo(CurrentOffset, TargetOffset, DeltaTime, 1.5f);
    FVector Location = Root->GetRelativeLocation();
    Location.X = CurrentOffset;
    Root->SetRelativeLocation(Location);
}
