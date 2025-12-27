#include "TherapyAffectDebugWidget.h"
#include "MetaHumanAffectControllerComponent.h"
#include "TherapyEmotionTypes.h"
#include "TherapyPatientDefinition.h"
#include "TherapyPatientDirector.h"
#include "TherapyPatientRegistrySubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

namespace
{
    void ApplyPresetValues(ETherapyAffectPacingPreset Preset, float& OutRiseSeconds, float& OutFallSeconds, float& OutMaxChangeRate)
    {
        switch (Preset)
        {
        case ETherapyAffectPacingPreset::HumanSlow:
            OutRiseSeconds = 1.2f;
            OutFallSeconds = 1.8f;
            OutMaxChangeRate = 1.0f;
            break;
        case ETherapyAffectPacingPreset::Fast:
            OutRiseSeconds = 0.15f;
            OutFallSeconds = 0.25f;
            OutMaxChangeRate = 5.0f;
            break;
        case ETherapyAffectPacingPreset::Default:
        default:
            OutRiseSeconds = 0.35f;
            OutFallSeconds = 0.5f;
            OutMaxChangeRate = 2.0f;
            break;
        }
    }
}

void UTherapyAffectDebugWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    TherapyEmotion::GetAllEmotionTags(OrderedEmotionTags);
}

void UTherapyAffectDebugWidget::NativeOnActivated()
{
    Super::NativeOnActivated();

    if (UGameInstance* GameInstance = GetGameInstance())
    {
        PatientRegistry = GameInstance->GetSubsystem<UTherapyPatientRegistrySubsystem>();
        if (PatientRegistry)
        {
            PatientRegistry->OnPatientSelectionChanged.AddDynamic(this, &UTherapyAffectDebugWidget::HandlePatientSelectionChanged);
        }
    }

    CachePatientList();
    BindToPatientDirector();
    RefreshCachedAffectData();

    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().SetTimer(RefreshTimerHandle, this, &UTherapyAffectDebugWidget::RefreshCachedAffectData, RefreshRateSeconds, true);
    }
}

void UTherapyAffectDebugWidget::NativeOnDeactivated()
{
    if (PatientRegistry)
    {
        PatientRegistry->OnPatientSelectionChanged.RemoveDynamic(this, &UTherapyAffectDebugWidget::HandlePatientSelectionChanged);
    }

    if (IsValid(PatientDirector))
    {
        PatientDirector->OnActiveAffectControllerChanged.RemoveDynamic(this, &UTherapyAffectDebugWidget::HandleActiveAffectControllerChanged);
    }

    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(RefreshTimerHandle);
    }

    Super::NativeOnDeactivated();
}

void UTherapyAffectDebugWidget::SetAffectController(UMetaHumanAffectControllerComponent* InController)
{
    AffectController = InController;
    RefreshCachedAffectData();
}

void UTherapyAffectDebugWidget::ApplyGlobalSpeed(float InMultiplier)
{
    if (AffectController)
    {
        AffectController->SetSpeedMultiplier(InMultiplier);
    }
}

void UTherapyAffectDebugWidget::ApplyPerEmotionSpeed(const FGameplayTag& EmotionTag, float RiseSeconds, float FallSeconds, float MaxChangeRate)
{
    if (AffectController)
    {
        AffectController->SetPerEmotionTuning(EmotionTag, RiseSeconds, FallSeconds, MaxChangeRate);
    }
}

void UTherapyAffectDebugWidget::ApplyPacingPreset(ETherapyAffectPacingPreset Preset)
{
    if (!AffectController)
    {
        return;
    }

    float RiseSeconds = 0.35f;
    float FallSeconds = 0.5f;
    float MaxChangeRate = 2.0f;
    ApplyPresetValues(Preset, RiseSeconds, FallSeconds, MaxChangeRate);

    for (const FGameplayTag& EmotionTag : OrderedEmotionTags)
    {
        AffectController->SetPerEmotionTuning(EmotionTag, RiseSeconds, FallSeconds, MaxChangeRate);
    }
}

void UTherapyAffectDebugWidget::ClearRuntimeTuningOverrides()
{
    if (AffectController)
    {
        AffectController->ClearRuntimeTuningOverrides();
    }
}

FTherapyPatientReaction UTherapyAffectDebugWidget::GetCurrentTargetReaction() const
{
    return CachedReaction;
}

TArray<float> UTherapyAffectDebugWidget::GetCurrentWeights() const
{
    return CachedWeights;
}

TArray<FGameplayTag> UTherapyAffectDebugWidget::GetOrderedEmotionTags() const
{
    return OrderedEmotionTags;
}

TArray<FText> UTherapyAffectDebugWidget::GetPatientDisplayNames() const
{
    return CachedPatientNames;
}

int32 UTherapyAffectDebugWidget::GetSelectedPatientIndex() const
{
    return CachedSelectedIndex;
}

void UTherapyAffectDebugWidget::SetSelectedPatientIndex(int32 Index)
{
    if (PatientRegistry)
    {
        PatientRegistry->SetCurrentPatientByIndex(Index);
    }
}

void UTherapyAffectDebugWidget::RefreshPatientRegistry()
{
    if (PatientRegistry)
    {
        PatientRegistry->RefreshRegistry();
        CachePatientList();
    }
}

float UTherapyAffectDebugWidget::GetCurrentSpeedMultiplier() const
{
    return AffectController ? AffectController->SpeedMultiplier : 1.0f;
}

void UTherapyAffectDebugWidget::BindToPatientDirector()
{
    if (IsValid(PatientDirector))
    {
        return;
    }

    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    TArray<AActor*> Directors;
    UGameplayStatics::GetAllActorsOfClass(World, ATherapyPatientDirector::StaticClass(), Directors);
    if (Directors.IsEmpty())
    {
        return;
    }

    PatientDirector = Cast<ATherapyPatientDirector>(Directors[0]);
    if (IsValid(PatientDirector))
    {
        PatientDirector->OnActiveAffectControllerChanged.AddDynamic(this, &UTherapyAffectDebugWidget::HandleActiveAffectControllerChanged);
        SetAffectController(PatientDirector->GetActiveAffectController());
    }
}

void UTherapyAffectDebugWidget::HandleActiveAffectControllerChanged(UMetaHumanAffectControllerComponent* NewController)
{
    SetAffectController(NewController);
}

void UTherapyAffectDebugWidget::HandlePatientSelectionChanged(UTherapyPatientDefinition* NewPatient)
{
    CachePatientList();
    if (!NewPatient)
    {
        return;
    }

    BindToPatientDirector();
}

void UTherapyAffectDebugWidget::RefreshCachedAffectData()
{
    if (!AffectController)
    {
        CachedReaction = FTherapyPatientReaction();
        CachedWeights.Reset();
        return;
    }

    CachedReaction = AffectController->GetCurrentTargetReaction();
    CachedWeights = AffectController->GetEmotionWeights();
}

void UTherapyAffectDebugWidget::CachePatientList()
{
    CachedPatientNames.Reset();
    CachedSelectedIndex = INDEX_NONE;

    if (!PatientRegistry)
    {
        return;
    }

    const TArray<UTherapyPatientDefinition*>& Patients = PatientRegistry->GetPatients();
    CachedPatientNames.Reserve(Patients.Num());

    UTherapyPatientDefinition* CurrentPatient = PatientRegistry->GetCurrentPatient();
    for (int32 Index = 0; Index < Patients.Num(); ++Index)
    {
        UTherapyPatientDefinition* Definition = Patients[Index];
        if (!Definition)
        {
            CachedPatientNames.Add(FText::FromString(TEXT("Unknown Patient")));
            continue;
        }

        const FText DisplayName = Definition->DisplayName.IsEmpty() ? FText::FromString(Definition->GetName()) : Definition->DisplayName;
        CachedPatientNames.Add(DisplayName);

        if (Definition == CurrentPatient)
        {
            CachedSelectedIndex = Index;
        }
    }
}
