#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "TherapyHudController.generated.h"

class UTherapyOverlayWidget;

UCLASS()
class THERAPYDP_API ATherapyHudController : public AHUD
{
    GENERATED_BODY()

public:
    virtual void BeginPlay() override;

    UPROPERTY(EditDefaultsOnly, Category="Therapy|UI")
    TSubclassOf<UTherapyOverlayWidget> OverlayWidgetClass;

    UPROPERTY(BlueprintReadOnly, Category="Therapy|UI")
    TObjectPtr<UTherapyOverlayWidget> OverlayWidget;
};
