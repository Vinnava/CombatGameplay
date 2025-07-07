// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Gameplay : ModuleRules
{
	public Gameplay(ReadOnlyTargetRules Target) : base(Target)
	{
		PrivateDependencyModuleNames.AddRange(new string[] { "AIModule", "MotionWarping", "GameplayCameras", "GameplayCameras" });
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "GameplayTags", "MotionTrajectory" });
	}
}
