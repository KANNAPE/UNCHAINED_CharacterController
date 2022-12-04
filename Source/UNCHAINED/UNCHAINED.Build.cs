// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class UNCHAINED : ModuleRules
{
	public UNCHAINED(ReadOnlyTargetRules Target) : base(Target)
	{
		PrivateDependencyModuleNames.AddRange(new string[] {"CableComponent"});
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });
	}
}
