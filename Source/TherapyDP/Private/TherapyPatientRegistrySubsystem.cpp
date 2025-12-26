#include "TherapyPatientRegistrySubsystem.h"
#include "TherapyPatientDefinition.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/AssetManager.h"
#include "Modules/ModuleManager.h"

void UTherapyPatientRegistrySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    EnsureDefaultSearchPath();
    RefreshRegistry();
}

void UTherapyPatientRegistrySubsystem::EnsureDefaultSearchPath()
{
    if (SearchPaths.Num() == 0)
    {
        FDirectoryPath DefaultPath;
        DefaultPath.Path = TEXT("/Game/TherapyDP/Characters/Patients");
        SearchPaths.Add(DefaultPath);
    }
}

void UTherapyPatientRegistrySubsystem::RefreshRegistry()
{
    PatientDefinitions.Reset();

    FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
    FARFilter Filter;
    Filter.ClassPaths.Add(UTherapyPatientDefinition::StaticClass()->GetClassPathName());
    Filter.bRecursiveClasses = true;

    for (const FDirectoryPath& Path : SearchPaths)
    {
        if (!Path.Path.IsEmpty())
        {
            Filter.PackagePaths.Add(*Path.Path);
        }
    }

    TArray<FAssetData> Assets;
    AssetRegistryModule.Get().GetAssets(Filter, Assets);

    for (const FAssetData& Asset : Assets)
    {
        if (UTherapyPatientDefinition* Definition = Cast<UTherapyPatientDefinition>(Asset.GetAsset()))
        {
            PatientDefinitions.Add(Definition);
        }
    }

    if (!CurrentPatient && PatientDefinitions.Num() > 0)
    {
        SetCurrentPatient(PatientDefinitions[0]);
    }
}

void UTherapyPatientRegistrySubsystem::SetCurrentPatient(UTherapyPatientDefinition* NewPatient)
{
    if (CurrentPatient == NewPatient)
    {
        return;
    }

    CurrentPatient = NewPatient;
    OnPatientSelectionChanged.Broadcast(CurrentPatient);
}

void UTherapyPatientRegistrySubsystem::SetCurrentPatientByIndex(int32 Index)
{
    if (!PatientDefinitions.IsValidIndex(Index))
    {
        return;
    }

    SetCurrentPatient(PatientDefinitions[Index]);
}
