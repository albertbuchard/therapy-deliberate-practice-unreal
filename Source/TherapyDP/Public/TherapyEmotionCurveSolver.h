#pragma once

#include "CoreMinimal.h"

struct FTherapyEmotionCurveParams
{
    float RiseSeconds = 0.35f;
    float FallSeconds = 0.5f;
    float HoldSeconds = 1.5f;
    float MaxChangeRate = 2.0f;
};

class FTherapyEmotionCurveSolver
{
public:
    void Initialize(int32 InNumEmotions);
    void Reset(float Value = 0.0f);
    void SetGlobalSpeedMultiplier(float InMultiplier);
    void SetParams(int32 Index, const FTherapyEmotionCurveParams& InParams);
    void SetTarget(int32 Index, float Target, float HoldSecondsOverride = -1.0f);
    void SetTargetWeight(int32 Index, float Target);
    void Tick(float DeltaSeconds);

    const TArray<float>& GetWeights() const { return Weights; }
    const TArray<float>& GetTargets() const { return Targets; }

private:
    float SolveStep(float Current, float Target, float DeltaSeconds, const FTherapyEmotionCurveParams& Params) const;

    float GlobalSpeedMultiplier = 1.0f;
    TArray<float> Weights;
    TArray<float> Targets;
    TArray<float> HoldRemaining;
    TArray<FTherapyEmotionCurveParams> Params;
};
