# Therapy Deliberate Practice Studio (UE 5.4)

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
1. Install Unreal Engine 5.4+.
2. Right click `TherapyDP.uproject` → Generate Visual Studio project files.
3. Open solution and build `Development Editor`.
4. Launch the project from UE.

### macOS
1. Install Unreal Engine 5.4+.
2. Right click `TherapyDP.uproject` → Generate Xcode project.
3. Open Xcode project, build `Development Editor`.
4. Launch the project from UE.

## Editor Setup

See `Content/TherapyDP/Docs/Setup.md` for content asset expectations.

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

- Gameplay tags define emotions/actions in `DefaultEngine.ini`.
- Use MetaHuman standard pipeline (Control Rig + AnimBP) for facial layers.
- CommonUI enabled for UI layers.
