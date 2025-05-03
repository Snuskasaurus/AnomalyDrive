// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class AnomalyDrive : ModuleRules
{
	public AnomalyDrive(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore",
			"VehicleSystemPlugin", 
			"EnhancedInput"
		});

		SetupModulePhysicsSupport(Target);
	}
}
