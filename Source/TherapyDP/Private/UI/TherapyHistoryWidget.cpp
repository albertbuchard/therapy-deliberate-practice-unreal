#include "UI/TherapyHistoryWidget.h"

void UTherapyHistoryWidget::SetHistory(const TArray<FTherapyAttemptSummary>& InHistory)
{
    History = InHistory;
}
