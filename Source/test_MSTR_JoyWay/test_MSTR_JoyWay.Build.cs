// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class test_MSTR_JoyWay : ModuleRules
{
	public test_MSTR_JoyWay(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });
	}
}
