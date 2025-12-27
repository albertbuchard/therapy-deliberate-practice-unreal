#include "UI/TherapyHudController.h"
#include "UI/TherapyOverlayWidget.h"
#include "Blueprint/UserWidget.h"

void ATherapyHudController::BeginPlay()
{
    Super::BeginPlay();

    if (!OverlayWidgetClass)
    {
        return;
    }

    OverlayWidget = CreateWidget<UTherapyOverlayWidget>(GetWorld(), OverlayWidgetClass);
    if (OverlayWidget)
    {
        OverlayWidget->AddToViewport();
        OverlayWidget->ActivateWidget();
    }
}
