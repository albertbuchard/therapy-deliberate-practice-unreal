#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TherapyTypes.h"
#include "TherapyCameraControllerComponent.generated.h"

UCLASS(ClassGroup=(TherapyDP), meta=(BlueprintSpawnableComponent))
class THERAPYDP_API UTherapyCameraControllerComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UTherapyCameraControllerComponent();

    UFUNCTION(BlueprintCallable)
    void ApplyReaction(const FTherapyPatientReaction& Reaction);

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
    float TargetOffset = 0.0f;
    float CurrentOffset = 0.0f;
};
