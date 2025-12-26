#include "ExerciseRepository.h"

void UExerciseRepository::Initialize(UTherapyApiClientSubsystem* InApiClient)
{
    ApiClient = InApiClient;
}

void UExerciseRepository::Refresh()
{
    if (!ApiClient)
    {
        return;
    }

    ApiClient->GetExercises(FTherapyApiExerciseList::CreateLambda([this](bool bSuccess, const TArray<FTherapyExercise>& Exercises)
    {
        if (bSuccess)
        {
            CachedExercises = Exercises;
            OnCacheUpdated.Broadcast(CachedExercises);
        }
    }));
}
