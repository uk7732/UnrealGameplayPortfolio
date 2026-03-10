// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using UnrealBuildTool.Rules;

public class FrontendUI : ModuleRules
{
	public FrontendUI(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"GameplayTags",
			"UMG",
			"CommonInput",
            "CommonUI",
            "PropertyPath",
			"PhysicsCore",
			"DeveloperSettings",
			"PreLoadScreen",
			"AIModule",
			"Slate",
			"SlateCore",
			"NavigationSystem",
			"GameplayTasks",
			"Niagara"
		
		});

		PublicIncludePaths.AddRange(
			new string[]
			{
            "FrontendUI/Public/Actors",
            "FrontendUI/Public/Actors/Item",
            "FrontendUI/Public/Actors/Projectile",
            "FrontendUI/Public/Actors/RangedEffect",
            });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		// Uncomment if you are using Slate UI
		PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore"  });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
