#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "TherapyTypes.generated.h"

UENUM(BlueprintType)
enum class ETherapySessionState : uint8
{
    Idle,
    Recording,
    Processing,
    Ready,
    Error
};

USTRUCT(BlueprintType)
struct FTherapyTranscriptWord
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Word;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float StartTime = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float EndTime = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Confidence = 0.0f;
};

USTRUCT(BlueprintType)
struct FTherapyTranscript
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Text;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Confidence = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FTherapyTranscriptWord> Words;
};

USTRUCT(BlueprintType)
struct FTherapyObjectiveScore
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString ObjectiveId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Score = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString RationaleShort;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FString> EvidenceQuotes;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FString> MissedPoints;
};

USTRUCT(BlueprintType)
struct FTherapyOverallScore
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Score = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bPass = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString SummaryFeedback;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FString> WhatToImproveNext;
};

USTRUCT(BlueprintType)
struct FTherapyPatientReaction
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FGameplayTag EmotionTag;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Intensity = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FGameplayTag ActionTag;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString ResponseText;
};

USTRUCT(BlueprintType)
struct FTherapyDiagnostics
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Provider;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 TimingMs = 0;
};

USTRUCT(BlueprintType)
struct FTherapyEvaluationResult
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Version;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString ExerciseId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString AttemptId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FTherapyTranscript Transcript;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FTherapyObjectiveScore> ObjectiveScores;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FTherapyOverallScore Overall;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FTherapyPatientReaction PatientReaction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TOptional<FTherapyDiagnostics> Diagnostics;
};

USTRUCT(BlueprintType)
struct FTherapyExercise
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Id;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Title;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FString> Objectives;
};
