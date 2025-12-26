# TherapyDP Content Setup

This folder stores content assets (maps, MetaHumans, UI widgets).

## UE 5.7 Setup Notes

- Open with Unreal Engine 5.7.
- Ensure plugins are enabled: **MetaHuman**, **Control Rig**, **Enhanced Input**, **CommonUI**.
- Gameplay tags live in `Config/DefaultGameplayTags.ini` (Project Settings → Gameplay Tags).

## Expected Assets

- `/Game/TherapyDP/Maps/TherapyDP_Office`
- `/Game/TherapyDP/Characters/MetaHumanPatient`
- `/Game/TherapyDP/UI/WBP_TherapyLibrary`
- `/Game/TherapyDP/UI/WBP_TherapyPractice`
- `/Game/TherapyDP/UI/WBP_TherapyHistory`
- `/Game/TherapyDP/Data/DA_EmotionMap`

Refer to README for detailed editor setup.

## MetaHuman Patient Wiring (Body + Face)

### 1) Create the Patient Blueprint
1. Import or create your MetaHuman (Quixel Bridge → MetaHuman).
2. Create a new blueprint:
   - **Class**: `Actor` (or use the MetaHuman blueprint as the parent).
   - **Name**: `BP_MetaHumanPatient`.
   - **Folder**: `/Game/TherapyDP/Characters/MetaHumanPatient`.
3. Add a tag: `TherapyPatient` (Actor → Tags).
4. Add component: `MetaHumanAffectControllerComponent`.

### 2) Mesh References
1. In `BP_MetaHumanPatient`, select the `MetaHumanAffectControllerComponent`.
2. Set:
   - **Body Mesh** → select the MetaHuman **Body** skeletal mesh component.
   - **Face Mesh** → select the MetaHuman **Face** skeletal mesh component.
   - (Optional) **Body Mesh Component Name** / **Face Mesh Component Name** if you prefer name-based resolution.

### 3) Animation Blueprints
1. **Body** skeletal mesh:
   - Anim Class → `ABP_TherapyPatientBody` (create if missing).
   - Parent Class → `UTherapyPatientAnimInstance`.
2. **Face** skeletal mesh:
   - Anim Class → `ABP_TherapyPatientFace` (create if missing).
   - Parent Class → `UTherapyPatientAnimInstance`.
3. If you already use a Post Process AnimBP for face, set its Parent Class to `UTherapyPatientAnimInstance` or use a face AnimBP that reads the same variables.

### 4) AnimGraph Guidance (Concrete Wiring)
Use `DA_EmotionMap` as the single source of tuning + asset mapping:
1. In `DA_EmotionMap`, fill:
   - **EmotionPoseAssets**: Map `neutral/warm/sad/anxious/angry/relieved/engaged` → pose assets.
   - **ActionAdditiveSequences**: Map `nod/shake_head/look_away/lean_in/sigh/smile` → additive sequences or montages.
2. In `ABP_TherapyPatientBody`:
   - **Base Pose**: use the pose asset from `EmotionTag` (Blend by enum/tag or DataTable lookup).
   - **EmotionBlend**: use as blend weight between neutral and target pose asset.
   - **Action overlay**: apply an additive sequence layer for `ActionTag` with **ActionBlend** as the alpha.
3. In `ABP_TherapyPatientFace`:
   - Drive facial rig controls from `EmotionTag`/`EmotionIntensity`.
   - Apply `ActionTag` additive on top of the facial pose layer for smiles or sighs.
4. Ensure `SetListening(true)` forces the neutral pose (EmotionTag = neutral, ActionTag cleared).

### 5) Level Wiring (C++ Route)
1. Place **BP_MetaHumanPatient** in `/Game/TherapyDP/Maps/TherapyDP_Office`.
2. Add a new actor: `TherapyPatientReactionRouter` (C++ class).
3. In the router:
   - Set **Patient Actor** to `BP_MetaHumanPatient` (or leave empty and rely on the `TherapyPatient` tag).
   - Optionally set **Patient Actor Class** to `BP_MetaHumanPatient` to auto-find.

### 6) Level Wiring (Blueprint-Only Alternative)
1. Create a blueprint actor (e.g., `BP_TherapySessionBinder`).
2. On `BeginPlay`:
   - Get `Game Instance` → `Get Subsystem (TherapySessionSubsystem)`.
   - Bind to `OnEvaluationReady`.
3. In the bound event:
   - `Get All Actors With Tag` → `TherapyPatient`.
   - Get `MetaHumanAffectControllerComponent` and call `ApplyReaction(Result.PatientReaction)`.
