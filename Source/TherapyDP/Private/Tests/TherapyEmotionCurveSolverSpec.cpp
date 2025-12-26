#include "Misc/AutomationTest.h"
#include "TherapyEmotionCurveSolver.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTherapyEmotionCurveSolverSpec, "TherapyDP.EmotionCurveSolver.ProgressesTowardTarget", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FTherapyEmotionCurveSolverSpec::RunTest(const FString& Parameters)
{
    FTherapyEmotionCurveSolver Solver;
    Solver.Initialize(1);

    FTherapyEmotionCurveParams Params;
    Params.RiseSeconds = 0.2f;
    Params.FallSeconds = 0.2f;
    Params.HoldSeconds = 0.0f;
    Params.MaxChangeRate = 10.0f;
    Solver.SetParams(0, Params);

    Solver.SetTarget(0, 1.0f, 0.0f);
    Solver.Tick(0.1f);

    const float Weight = Solver.GetWeights()[0];
    TestTrue(TEXT("Weight should move toward target"), Weight > 0.0f && Weight < 1.0f);

    Solver.SetTargetWeight(0, 0.0f);
    Solver.Tick(0.1f);

    const float LoweredWeight = Solver.GetWeights()[0];
    TestTrue(TEXT("Weight should decay toward zero"), LoweredWeight < Weight);

    return true;
}
