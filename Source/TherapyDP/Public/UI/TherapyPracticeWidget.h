#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "TherapyTypes.h"
#include "TherapyPracticeWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTherapyRecordPressed);

UCLASS()
class THERAPYDP_API UTherapyPracticeWidget : public UCommonActivatableWidget
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadOnly, Category = "Therapy")
    FTherapyEvaluationResult CurrentResult;

    UPROPERTY(BlueprintAssignable, Category = "Therapy")
    FTherapyRecordPressed OnRecordPressed;

    UFUNCTION(BlueprintCallable)
    void SetEvaluationResult(const FTherapyEvaluationResult& Result);
};
