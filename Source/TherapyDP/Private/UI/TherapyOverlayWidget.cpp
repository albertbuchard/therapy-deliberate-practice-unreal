#include "UI/TherapyOverlayWidget.h"
#include "CommonActivatableWidgetSwitcher.h"
#include "CommonButtonBase.h"
#include "CommonTabListWidgetBase.h"
#include "TherapyAffectDebugWidget.h"
#include "UI/TherapyHistoryWidget.h"
#include "UI/TherapyLibraryWidget.h"
#include "UI/TherapyPracticeWidget.h"

void UTherapyOverlayWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    InitializeTabs();
}

void UTherapyOverlayWidget::InitializeTabs()
{
    if (!TopNavTabs || !ContentSwitcher)
    {
        return;
    }

    TopNavTabs->SetLinkedSwitcher(ContentSwitcher);

    RegisterTab(TEXT("Library"), LibraryWidgetClass);
    RegisterTab(TEXT("Practice"), PracticeWidgetClass);
    RegisterTab(TEXT("History"), HistoryWidgetClass);
    RegisterTab(TEXT("Debug"), DebugWidgetClass);

    if (LibraryWidgetClass)
    {
        TopNavTabs->SelectTabByID(TEXT("Library"));
    }
    else if (PracticeWidgetClass)
    {
        TopNavTabs->SelectTabByID(TEXT("Practice"));
    }
    else if (HistoryWidgetClass)
    {
        TopNavTabs->SelectTabByID(TEXT("History"));
    }
    else if (DebugWidgetClass)
    {
        TopNavTabs->SelectTabByID(TEXT("Debug"));
    }
}

UCommonActivatableWidget* UTherapyOverlayWidget::GetActiveTabWidget() const
{
    return ContentSwitcher ? Cast<UCommonActivatableWidget>(ContentSwitcher->GetActiveWidget()) : nullptr;
}

void UTherapyOverlayWidget::RegisterTab(const FName TabName, TSubclassOf<UCommonActivatableWidget> WidgetClass)
{
    if (!TopNavTabs || !TabButtonClass || !WidgetClass)
    {
        return;
    }

    TopNavTabs->RegisterTab(TabName, TabButtonClass, WidgetClass);
}
