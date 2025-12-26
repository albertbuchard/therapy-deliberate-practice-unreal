#include "TherapyPatientReactionRouter.h"
#include "MetaHumanAffectControllerComponent.h"
#include "TherapySessionSubsystem.h"
#include "Kismet/GameplayStatics.h"

ATherapyPatientReactionRouter::ATherapyPatientReactionRouter()
{
    PrimaryActorTick.bCanEverTick = false;
}

void ATherapyPatientReactionRouter::BeginPlay()
{
    Super::BeginPlay();

    CachePatientComponent();

    if (UGameInstance* GameInstance = GetGameInstance())
    {
        SessionSubsystem = GameInstance->GetSubsystem<UTherapySessionSubsystem>();
        if (SessionSubsystem)
        {
            SessionSubsystem->OnEvaluationReady.AddDynamic(this, &ATherapyPatientReactionRouter::HandleEvaluationReady);
        }
    }
}

void ATherapyPatientReactionRouter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (SessionSubsystem)
    {
        SessionSubsystem->OnEvaluationReady.RemoveDynamic(this, &ATherapyPatientReactionRouter::HandleEvaluationReady);
    }

    Super::EndPlay(EndPlayReason);
}

void ATherapyPatientReactionRouter::HandleEvaluationReady(const FTherapyEvaluationResult& Result)
{
    if (!PatientAffectComponent.IsValid())
    {
        CachePatientComponent();
    }

    if (PatientAffectComponent.IsValid())
    {
        PatientAffectComponent->ApplyReaction(Result.PatientReaction);
    }
}

void ATherapyPatientReactionRouter::CachePatientComponent()
{
    AActor* TargetActor = PatientActor;

    if (!TargetActor && bAutoFindPatient)
    {
        if (!PatientActorTag.IsNone())
        {
            TArray<AActor*> TaggedActors;
            UGameplayStatics::GetAllActorsWithTag(this, PatientActorTag, TaggedActors);
            if (TaggedActors.Num() > 0)
            {
                TargetActor = TaggedActors[0];
            }
        }

        if (!TargetActor && PatientActorClass)
        {
            TArray<AActor*> ClassActors;
            UGameplayStatics::GetAllActorsOfClass(this, PatientActorClass, ClassActors);
            if (ClassActors.Num() > 0)
            {
                TargetActor = ClassActors[0];
            }
        }
    }

    PatientAffectComponent = TargetActor
        ? TargetActor->FindComponentByClass<UMetaHumanAffectControllerComponent>()
        : nullptr;
}
