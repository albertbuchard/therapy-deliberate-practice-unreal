#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "TherapyApiClientSubsystem.h"
#include "ExerciseRepository.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTherapyExerciseCacheUpdated, const TArray<FTherapyExercise>&, Exercises);

UCLASS(BlueprintType)
class THERAPYDP_API UExerciseRepository : public UObject
{
    GENERATED_BODY()

public:
    void Initialize(UTherapyApiClientSubsystem* InApiClient);

    UFUNCTION(BlueprintCallable)
    void Refresh();

    UFUNCTION(BlueprintCallable)
    const TArray<FTherapyExercise>& GetCachedExercises() const { return CachedExercises; }

    UPROPERTY(BlueprintAssignable)
    FTherapyExerciseCacheUpdated OnCacheUpdated;

private:
    UPROPERTY()
    UTherapyApiClientSubsystem* ApiClient = nullptr;

    UPROPERTY()
    TArray<FTherapyExercise> CachedExercises;
};
