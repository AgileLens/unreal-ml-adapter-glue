using UnrealBuildTool;

public class MLAdapter : ModuleRules
{
    public MLAdapter(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicIncludePaths.AddRange(
            new string[] {
                // ... add public include paths here ...
            }
        );
        PrivateIncludePaths.AddRange(
            new string[] {
                // ... add other private include paths ...
            }
        );
        PublicDependencyModuleNames.AddRange(
            new string[] {
                "Core",
                "CoreUObject",
                "Engine",
                "Sockets",
                "Networking"
            }
        );
        PrivateDependencyModuleNames.AddRange(
            new string[] {
                // ... add private dependencies that you statically link with here ...
            }
        );
        DynamicallyLoadedModuleNames.AddRange(
            new string[] {
                // ...
            }
        );
    }
}