#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "TherapyOverlayWidget.generated.h"

class UCommonActivatableWidgetSwitcher;
class UCommonButtonBase;
class UCommonTabListWidgetBase;
class UTherapyAffectDebugWidget;
class UTherapyHistoryWidget;
class UTherapyLibraryWidget;
class UTherapyPracticeWidget;

UCLASS()
class THERAPYDP_API UTherapyOverlayWidget : public UCommonActivatableWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    void InitializeTabs();

    UFUNCTION(BlueprintCallable)
    UCommonActivatableWidget* GetActiveTabWidget() const;

protected:
    virtual void NativeOnInitialized() override;

private:
    void RegisterTab(const FName TabName, TSubclassOf<UCommonActivatableWidget> WidgetClass);

    UPROPERTY(EditDefaultsOnly, Category="Therapy|UI")
    TSubclassOf<UCommonButtonBase> TabButtonClass;

    UPROPERTY(EditDefaultsOnly, Category="Therapy|UI")
    TSubclassOf<UTherapyLibraryWidget> LibraryWidgetClass;

    UPROPERTY(EditDefaultsOnly, Category="Therapy|UI")
    TSubclassOf<UTherapyPracticeWidget> PracticeWidgetClass;

    UPROPERTY(EditDefaultsOnly, Category="Therapy|UI")
    TSubclassOf<UTherapyHistoryWidget> HistoryWidgetClass;

    UPROPERTY(EditDefaultsOnly, Category="Therapy|UI")
    TSubclassOf<UTherapyAffectDebugWidget> DebugWidgetClass;

    UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional, AllowPrivateAccess="true"))
    TObjectPtr<UCommonTabListWidgetBase> TopNavTabs;

    UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional, AllowPrivateAccess="true"))
    TObjectPtr<UCommonActivatableWidgetSwitcher> ContentSwitcher;
};
