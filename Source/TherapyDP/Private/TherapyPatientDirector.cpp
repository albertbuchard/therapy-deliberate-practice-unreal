#include "TherapyPatientDirector.h"
#include "MetaHumanAffectControllerComponent.h"
#include "TherapyPatientDefinition.h"
#include "TherapyPatientRegistrySubsystem.h"
#include "TherapySessionSubsystem.h"
#include "Engine/World.h"

ATherapyPatientDirector::ATherapyPatientDirector()
{
    PrimaryActorTick.bCanEverTick = false;
}

void ATherapyPatientDirector::BeginPlay()
{
    Super::BeginPlay();

    if (UGameInstance* GameInstance = GetGameInstance())
    {
        SessionSubsystem = GameInstance->GetSubsystem<UTherapySessionSubsystem>();
        PatientRegistry = GameInstance->GetSubsystem<UTherapyPatientRegistrySubsystem>();

        if (SessionSubsystem)
        {
            SessionSubsystem->OnEvaluationReady.AddDynamic(this, &ATherapyPatientDirector::HandleEvaluationReady);
            SessionSubsystem->OnStateChanged.AddDynamic(this, &ATherapyPatientDirector::HandleSessionStateChanged);
            HandleSessionStateChanged(SessionSubsystem->GetState());
        }

        if (PatientRegistry)
        {
            PatientRegistry->OnPatientSelectionChanged.AddDynamic(this, &ATherapyPatientDirector::HandlePatientSelectionChanged);
        }
    }

    RefreshPatientActor();
    CachePatientComponent();
}

void ATherapyPatientDirector::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (SessionSubsystem)
    {
        SessionSubsystem->OnEvaluationReady.RemoveDynamic(this, &ATherapyPatientDirector::HandleEvaluationReady);
        SessionSubsystem->OnStateChanged.RemoveDynamic(this, &ATherapyPatientDirector::HandleSessionStateChanged);
    }

    if (PatientRegistry)
    {
        PatientRegistry->OnPatientSelectionChanged.RemoveDynamic(this, &ATherapyPatientDirector::HandlePatientSelectionChanged);
    }

    Super::EndPlay(EndPlayReason);
}

void ATherapyPatientDirector::HandleEvaluationReady(const FTherapyEvaluationResult& Result)
{
    if (!PatientAffectComponent.IsValid())
    {
        CachePatientComponent();
    }

    if (PatientAffectComponent.IsValid())
    {
        PatientAffectComponent->SetListening(false);
        PatientAffectComponent->ApplyReaction(Result.PatientReaction);
    }
}

void ATherapyPatientDirector::HandleSessionStateChanged(ETherapySessionState NewState)
{
    const bool bListening = (NewState == ETherapySessionState::Recording);
    if (PatientAffectComponent.IsValid())
    {
        PatientAffectComponent->SetListening(bListening);
    }
}

void ATherapyPatientDirector::HandlePatientSelectionChanged(UTherapyPatientDefinition* NewPatient)
{
    RefreshPatientActor();
    CachePatientComponent();

    if (PatientAffectComponent.IsValid() && NewPatient)
    {
        PatientAffectComponent->EmotionMap = NewPatient->EmotionMap;
        if (SessionSubsystem)
        {
            HandleSessionStateChanged(SessionSubsystem->GetState());
        }
    }
}

void ATherapyPatientDirector::RefreshPatientActor()
{
    if (PatientActorOverride)
    {
        ActivePatientActor = PatientActorOverride;
        return;
    }

    if (!PatientRegistry || !bAutoSpawnSelectedPatient)
    {
        return;
    }

    UTherapyPatientDefinition* Definition = PatientRegistry->GetCurrentPatient();
    if (!Definition)
    {
        return;
    }

    if (ActivePatientActor && bDestroyPreviousPatientOnSelection)
    {
        ActivePatientActor->Destroy();
        ActivePatientActor = nullptr;
    }

    if (Definition->PatientActorClass.IsNull())
    {
        return;
    }

    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    TSubclassOf<AActor> PatientClass = Definition->PatientActorClass.LoadSynchronous();
    if (!PatientClass)
    {
        return;
    }

    const FTransform SpawnTransform = PatientSpawnTransform.Equals(FTransform::Identity) ? GetActorTransform() : PatientSpawnTransform;
    ActivePatientActor = World->SpawnActor<AActor>(PatientClass, SpawnTransform);
}

void ATherapyPatientDirector::CachePatientComponent()
{
    AActor* TargetActor = PatientActorOverride ? PatientActorOverride : ActivePatientActor;
    PatientAffectComponent = TargetActor ? TargetActor->FindComponentByClass<UMetaHumanAffectControllerComponent>() : nullptr;

    if (PatientAffectComponent.IsValid() && PatientRegistry)
    {
        if (UTherapyPatientDefinition* Definition = PatientRegistry->GetCurrentPatient())
        {
            PatientAffectComponent->EmotionMap = Definition->EmotionMap;
        }
    }
}
