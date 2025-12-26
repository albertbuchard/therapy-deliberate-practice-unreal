#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "TherapyTypes.h"
#include "TherapyLibraryWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTherapyExerciseSelected, const FString&, ExerciseId);

UCLASS()
class THERAPYDP_API UTherapyLibraryWidget : public UCommonActivatableWidget
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadOnly, Category = "Therapy")
    TArray<FTherapyExercise> Exercises;

    UPROPERTY(BlueprintAssignable, Category = "Therapy")
    FTherapyExerciseSelected OnExerciseSelected;

    UFUNCTION(BlueprintCallable)
    void SetExercises(const TArray<FTherapyExercise>& InExercises);
};
