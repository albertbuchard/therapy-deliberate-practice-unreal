#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "TherapyTypes.h"
#include "AttemptHistoryService.generated.h"

USTRUCT(BlueprintType)
struct FTherapyAttemptSummary
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString AttemptId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString ExerciseId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FDateTime Timestamp;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Score = 0;
};

UCLASS(BlueprintType)
class THERAPYDP_API UAttemptHistoryService : public UObject
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    void AddAttempt(const FTherapyEvaluationResult& Result);

    UFUNCTION(BlueprintCallable)
    const TArray<FTherapyAttemptSummary>& GetHistory() const { return History; }

private:
    UPROPERTY()
    TArray<FTherapyAttemptSummary> History;
};
