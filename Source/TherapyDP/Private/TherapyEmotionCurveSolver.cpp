#include "TherapyEmotionCurveSolver.h"

void FTherapyEmotionCurveSolver::Initialize(int32 InNumEmotions)
{
    Weights.Init(0.0f, InNumEmotions);
    Targets.Init(0.0f, InNumEmotions);
    HoldRemaining.Init(0.0f, InNumEmotions);
    Params.Init(FTherapyEmotionCurveParams(), InNumEmotions);
}

void FTherapyEmotionCurveSolver::Reset(float Value)
{
    for (float& Weight : Weights)
    {
        Weight = Value;
    }

    for (float& Target : Targets)
    {
        Target = Value;
    }

    for (float& Hold : HoldRemaining)
    {
        Hold = 0.0f;
    }
}

void FTherapyEmotionCurveSolver::SetGlobalSpeedMultiplier(float InMultiplier)
{
    GlobalSpeedMultiplier = FMath::Clamp(InMultiplier, 0.01f, 10.0f);
}

void FTherapyEmotionCurveSolver::SetParams(int32 Index, const FTherapyEmotionCurveParams& InParams)
{
    if (Params.IsValidIndex(Index))
    {
        Params[Index] = InParams;
    }
}

void FTherapyEmotionCurveSolver::SetTarget(int32 Index, float Target, float HoldSecondsOverride)
{
    if (!Targets.IsValidIndex(Index))
    {
        return;
    }

    Targets[Index] = FMath::Clamp(Target, 0.0f, 1.0f);
    const float HoldSeconds = HoldSecondsOverride >= 0.0f ? HoldSecondsOverride : Params[Index].HoldSeconds;
    HoldRemaining[Index] = FMath::Max(0.0f, HoldSeconds);
}

void FTherapyEmotionCurveSolver::SetTargetWeight(int32 Index, float Target)
{
    if (Targets.IsValidIndex(Index))
    {
        Targets[Index] = FMath::Clamp(Target, 0.0f, 1.0f);
    }
}

void FTherapyEmotionCurveSolver::Tick(float DeltaSeconds)
{
    if (Weights.Num() == 0)
    {
        return;
    }

    const float EffectiveDelta = DeltaSeconds * GlobalSpeedMultiplier;

    for (int32 Index = 0; Index < Weights.Num(); ++Index)
    {
        float Current = Weights[Index];
        float Target = Targets[Index];

        if (HoldRemaining[Index] > 0.0f)
        {
            HoldRemaining[Index] = FMath::Max(0.0f, HoldRemaining[Index] - EffectiveDelta);
            if (Target < Current)
            {
                Target = Current;
            }
        }

        Weights[Index] = SolveStep(Current, Target, EffectiveDelta, Params[Index]);
    }
}

float FTherapyEmotionCurveSolver::SolveStep(float Current, float Target, float DeltaSeconds, const FTherapyEmotionCurveParams& InParams) const
{
    const float RiseSeconds = FMath::Max(0.01f, InParams.RiseSeconds);
    const float FallSeconds = FMath::Max(0.01f, InParams.FallSeconds);
    const float Tau = (Target >= Current) ? RiseSeconds : FallSeconds;

    const float Alpha = 1.0f - FMath::Exp(-DeltaSeconds / Tau);
    float NewValue = FMath::Lerp(Current, Target, Alpha);

    const float MaxDelta = FMath::Max(0.0f, InParams.MaxChangeRate) * DeltaSeconds;
    if (MaxDelta > 0.0f)
    {
        NewValue = Current + FMath::Clamp(NewValue - Current, -MaxDelta, MaxDelta);
    }

    return FMath::Clamp(NewValue, 0.0f, 1.0f);
}
