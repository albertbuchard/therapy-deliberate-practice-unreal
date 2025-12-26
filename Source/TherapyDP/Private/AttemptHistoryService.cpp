#include "AttemptHistoryService.h"

void UAttemptHistoryService::AddAttempt(const FTherapyEvaluationResult& Result)
{
    FTherapyAttemptSummary Summary;
    Summary.AttemptId = Result.AttemptId;
    Summary.ExerciseId = Result.ExerciseId;
    Summary.Timestamp = FDateTime::UtcNow();
    Summary.Score = Result.Overall.Score;
    History.Insert(Summary, 0);
}
