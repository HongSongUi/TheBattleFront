// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TheBattleFront : ModuleRules
{
	public TheBattleFront(ReadOnlyTargetRules Target) : base(Target)
	{
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "NavigationSystem", "AIModule", "Niagara", "EnhancedInput", "UMG", "GameplayTasks", "Sockets", "Networking", "GeometryCollectionEngine", "Paper2D", "SlateCore" });
        PrivateDependencyModuleNames.AddRange(new string[] { "ProtobufCore", "Paper2D" });
        PrivateIncludePaths.AddRange(new string[]
        {

            "TheBattleFront/",
            "TheBattleFront/Network/",
            "TheBattleFront/Network/Test/",
            "TheBattleFront/Team_AI",
            "TheBattleFront/Team_AI/Public/",
            "TheBattleFront/Team_AI/Private/",
        });
    }
}
