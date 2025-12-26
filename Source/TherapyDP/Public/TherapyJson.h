#pragma once

#include "CoreMinimal.h"
#include "TherapyTypes.h"

namespace TherapyJson
{
    bool ParseExercise(const TSharedPtr<FJsonObject>& JsonObject, FTherapyExercise& OutExercise);
    bool ParseExerciseArray(const TArray<TSharedPtr<FJsonValue>>& JsonArray, TArray<FTherapyExercise>& OutExercises);
    bool ParseEvaluation(const TSharedPtr<FJsonObject>& JsonObject, FTherapyEvaluationResult& OutResult);
    TSharedPtr<FJsonObject> BuildPracticeRunPayload(const FString& ExerciseId, const FString& AttemptId, const FString& AudioBase64, const FString& Mode);
}
