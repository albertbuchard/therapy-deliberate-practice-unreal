# Therapy Deliberate Practice Studio (UE 5.7)

Production-grade Unreal project scaffolding for the Therapy Deliberate Practice Studio. This repo focuses on clean C++ systems, runtime module layout, and editor setup guidance for MetaHuman-driven sessions.

## Architecture Overview

**Runtime module**: `TherapyDP`

Key systems (C++ first):

- `UTherapyApiClientSubsystem`: HTTP client, retries/backoff, JSON parsing.
- `UTherapySessionSubsystem`: deterministic state machine for the session lifecycle.
- `UExerciseRepository`: exercise library cache + refresh.
- `UAttemptHistoryService`: lightweight local attempt history.
- `UMetaHumanAffectControllerComponent`: drives animation parameters for face/body.
- `UTherapyCameraControllerComponent`: subtle therapist POV camera drift.

## Folder Structure

```
/Config
  DefaultEngine.ini
  DefaultGame.ini
/Content
  /TherapyDP
    /Docs
/Source
  /TherapyDP
    /Public
    /Private
TherapyDP.uproject
```

## Build + Run

### Windows
1. Install Unreal Engine 5.7.
2. Right click `TherapyDP.uproject` → Generate Visual Studio project files.
3. Open solution and build `Development Editor`.
4. Launch the project from UE.

### macOS
1. Install Unreal Engine 5.7.
2. Right click `TherapyDP.uproject` → Generate Xcode project.
3. Open Xcode project, build `Development Editor`.
4. Launch the project from UE.

## Editor Setup

See `Content/TherapyDP/Docs/Setup.md` for content asset expectations.

## UE 5.7 Upgrade Checklist

1. Update `.uproject`:
   - `EngineAssociation` must be `5.7`.
   - `MetaHuman` plugin name casing should be `MetaHuman`.
2. Confirm plugins:
   - `EnhancedInput`, `CommonUI`, `MetaHuman`, and `ControlRig` enabled.
3. Gameplay tags:
   - Tags live in `Config/DefaultGameplayTags.ini`.
   - `Project Settings → Gameplay Tags` should list neutral/warm/sad/anxious/angry/relieved/engaged + action tags.
4. Rebuild C++:
   - Regenerate project files and build `TherapyDP` in `Development Editor`.
5. MetaHuman wiring:
   - Ensure the patient blueprint uses `UTherapyPatientAnimInstance` for body/face anim BPs.
   - Add `UMetaHumanAffectControllerComponent` and set body/face mesh references.
6. Package validation:
   - Run a Development build and verify no missing module or plugin warnings.

## UE 5.7 API Impact Notes

Current TherapyDP code paths (Subsystems, HTTP, Json, TimerManager, GameplayTags, CommonUI, AudioCapture) are compatible with UE 5.7. The only required changes for 5.7 are:

- MetaHuman plugin casing update (`MetaHuman`).
- Gameplay tags moved to `DefaultGameplayTags.ini`.
- MetaHuman mesh resolution updated to avoid `FindComponentByClass` ambiguity.

## Local API Config

Adjust the following in `Config/DefaultGame.ini`:

```
[/Script/TherapyDP.TherapyDPSettings]
BaseUrl=http://localhost:8000/
AiMode=local_prefer
OpenAiApiKey=
LocalSttUrl=http://localhost:8081/
LocalLlmUrl=http://localhost:8082/
LocalLlmModel=local-model
DbPath=
```

## Notes

- Gameplay tags define emotions/actions in `DefaultGameplayTags.ini`.
- Use MetaHuman standard pipeline (Control Rig + AnimBP) for facial layers.
- CommonUI enabled for UI layers.
