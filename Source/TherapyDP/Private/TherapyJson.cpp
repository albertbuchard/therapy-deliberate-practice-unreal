#include "TherapyJson.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "TherapyDP.h"

namespace
{
    FGameplayTag TagFromString(const FString& TagString)
    {
        if (TagString.IsEmpty())
        {
            return FGameplayTag();
        }
        return FGameplayTag::RequestGameplayTag(FName(*TagString), false);
    }

    FString StringFromTag(const FGameplayTag& Tag)
    {
        return Tag.IsValid() ? Tag.ToString() : FString();
    }
}

namespace TherapyJson
{
    bool ParseExercise(const TSharedPtr<FJsonObject>& JsonObject, FTherapyExercise& OutExercise)
    {
        if (!JsonObject.IsValid())
        {
            return false;
        }

        JsonObject->TryGetStringField(TEXT("id"), OutExercise.Id);
        JsonObject->TryGetStringField(TEXT("title"), OutExercise.Title);
        JsonObject->TryGetStringField(TEXT("description"), OutExercise.Description);
        OutExercise.Objectives.Empty();
        const TArray<TSharedPtr<FJsonValue>>* ObjectivesArray;
        if (JsonObject->TryGetArrayField(TEXT("objectives"), ObjectivesArray))
        {
            for (const TSharedPtr<FJsonValue>& Value : *ObjectivesArray)
            {
                OutExercise.Objectives.Add(Value->AsString());
            }
        }
        return true;
    }

    bool ParseExerciseArray(const TArray<TSharedPtr<FJsonValue>>& JsonArray, TArray<FTherapyExercise>& OutExercises)
    {
        OutExercises.Empty();
        for (const TSharedPtr<FJsonValue>& Value : JsonArray)
        {
            const TSharedPtr<FJsonObject> Obj = Value->AsObject();
            FTherapyExercise Exercise;
            if (ParseExercise(Obj, Exercise))
            {
                OutExercises.Add(Exercise);
            }
        }
        return OutExercises.Num() > 0;
    }

    bool ParseEvaluation(const TSharedPtr<FJsonObject>& JsonObject, FTherapyEvaluationResult& OutResult)
    {
        if (!JsonObject.IsValid())
        {
            return false;
        }

        JsonObject->TryGetStringField(TEXT("version"), OutResult.Version);
        JsonObject->TryGetStringField(TEXT("exercise_id"), OutResult.ExerciseId);
        JsonObject->TryGetStringField(TEXT("attempt_id"), OutResult.AttemptId);

        if (JsonObject->HasField(TEXT("transcript")))
        {
            const TSharedPtr<FJsonObject> TranscriptObj = JsonObject->GetObjectField(TEXT("transcript"));
            TranscriptObj->TryGetStringField(TEXT("text"), OutResult.Transcript.Text);
            TranscriptObj->TryGetNumberField(TEXT("confidence"), OutResult.Transcript.Confidence);
            OutResult.Transcript.Words.Empty();
            const TArray<TSharedPtr<FJsonValue>>* Words;
            if (TranscriptObj->TryGetArrayField(TEXT("words"), Words))
            {
                for (const TSharedPtr<FJsonValue>& WordValue : *Words)
                {
                    FTherapyTranscriptWord Word;
                    const TSharedPtr<FJsonObject> WordObj = WordValue->AsObject();
                    WordObj->TryGetStringField(TEXT("w"), Word.Word);
                    WordObj->TryGetNumberField(TEXT("t0"), Word.StartTime);
                    WordObj->TryGetNumberField(TEXT("t1"), Word.EndTime);
                    WordObj->TryGetNumberField(TEXT("p"), Word.Confidence);
                    OutResult.Transcript.Words.Add(Word);
                }
            }
        }

        const TArray<TSharedPtr<FJsonValue>>* ObjectiveArray;
        if (JsonObject->TryGetArrayField(TEXT("objective_scores"), ObjectiveArray))
        {
            OutResult.ObjectiveScores.Empty();
            for (const TSharedPtr<FJsonValue>& ObjValue : *ObjectiveArray)
            {
                const TSharedPtr<FJsonObject> Obj = ObjValue->AsObject();
                FTherapyObjectiveScore Score;
                Score.ObjectiveId = Obj->GetStringField(TEXT("objective_id"));
                Score.Score = Obj->GetIntegerField(TEXT("score"));
                Score.RationaleShort = Obj->GetStringField(TEXT("rationale_short"));
                const TArray<TSharedPtr<FJsonValue>>* Evidence;
                if (Obj->TryGetArrayField(TEXT("evidence_quotes"), Evidence))
                {
                    for (const TSharedPtr<FJsonValue>& EvidenceValue : *Evidence)
                    {
                        Score.EvidenceQuotes.Add(EvidenceValue->AsString());
                    }
                }
                const TArray<TSharedPtr<FJsonValue>>* Missed;
                if (Obj->TryGetArrayField(TEXT("missed_points"), Missed))
                {
                    for (const TSharedPtr<FJsonValue>& MissedValue : *Missed)
                    {
                        Score.MissedPoints.Add(MissedValue->AsString());
                    }
                }
                OutResult.ObjectiveScores.Add(Score);
            }
        }

        const TSharedPtr<FJsonObject> OverallObj = JsonObject->GetObjectField(TEXT("overall"));
        OverallObj->TryGetNumberField(TEXT("score"), OutResult.Overall.Score);
        OverallObj->TryGetBoolField(TEXT("pass"), OutResult.Overall.bPass);
        OverallObj->TryGetStringField(TEXT("summary_feedback"), OutResult.Overall.SummaryFeedback);
        const TArray<TSharedPtr<FJsonValue>>* Improve;
        if (OverallObj->TryGetArrayField(TEXT("what_to_improve_next"), Improve))
        {
            for (const TSharedPtr<FJsonValue>& ImproveValue : *Improve)
            {
                OutResult.Overall.WhatToImproveNext.Add(ImproveValue->AsString());
            }
        }

        const TSharedPtr<FJsonObject> ReactionObj = JsonObject->GetObjectField(TEXT("patient_reaction"));
        FString EmotionString;
        FString ActionString;
        ReactionObj->TryGetStringField(TEXT("emotion"), EmotionString);
        ReactionObj->TryGetStringField(TEXT("action"), ActionString);
        OutResult.PatientReaction.EmotionTag = TagFromString(EmotionString);
        ReactionObj->TryGetNumberField(TEXT("intensity"), OutResult.PatientReaction.Intensity);
        OutResult.PatientReaction.ActionTag = TagFromString(ActionString);
        ReactionObj->TryGetStringField(TEXT("response_text"), OutResult.PatientReaction.ResponseText);

        if (JsonObject->HasField(TEXT("diagnostics")))
        {
            FTherapyDiagnostics Diagnostics;
            const TSharedPtr<FJsonObject> DiagnosticsObj = JsonObject->GetObjectField(TEXT("diagnostics"));
            Diagnostics.Provider = DiagnosticsObj->GetStringField(TEXT("provider"));
            Diagnostics.TimingMs = DiagnosticsObj->GetIntegerField(TEXT("timing_ms"));
            OutResult.Diagnostics = Diagnostics;
        }
        else
        {
            OutResult.Diagnostics.Reset();
        }

        return true;
    }

    TSharedPtr<FJsonObject> BuildPracticeRunPayload(const FString& ExerciseId, const FString& AttemptId, const FString& AudioBase64, const FString& Mode)
    {
        TSharedPtr<FJsonObject> Root = MakeShared<FJsonObject>();
        Root->SetStringField(TEXT("exercise_id"), ExerciseId);
        if (!AttemptId.IsEmpty())
        {
            Root->SetStringField(TEXT("attempt_id"), AttemptId);
        }
        Root->SetStringField(TEXT("audio"), AudioBase64);
        if (!Mode.IsEmpty())
        {
            Root->SetStringField(TEXT("mode"), Mode);
        }
        return Root;
    }
}
