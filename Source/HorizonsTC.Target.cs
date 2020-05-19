// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class HorizonsTCTarget : TargetRules
{
	public HorizonsTCTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		ExtraModuleNames.AddRange(new string[] {"HorizonsTC"});

		bIWYU = true;
		bLegacyPublicIncludePaths = false;
	}
}