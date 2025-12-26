using UnrealBuildTool;

public class TherapyDP : ModuleRules
{
    public TherapyDP(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "InputCore",
                "HTTP",
                "Json",
                "JsonUtilities",
                "GameplayTags",
                "EnhancedInput",
                "CommonUI",
                "AudioCapture",
                "UMG"
            }
        );

        PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore", "AssetRegistry" });
    }
}
