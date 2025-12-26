#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "AttemptHistoryService.h"
#include "TherapyHistoryWidget.generated.h"

UCLASS()
class THERAPYDP_API UTherapyHistoryWidget : public UCommonActivatableWidget
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadOnly, Category = "Therapy")
    TArray<FTherapyAttemptSummary> History;

    UFUNCTION(BlueprintCallable)
    void SetHistory(const TArray<FTherapyAttemptSummary>& InHistory);
};
