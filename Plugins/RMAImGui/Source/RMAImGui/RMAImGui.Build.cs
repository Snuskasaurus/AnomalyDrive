// Copyright 2017-2025 Rafael Marques Almeida. All Rights Reserved.
using UnrealBuildTool;
using System.IO;
using System;

public class RMAImGui : ModuleRules
{
	public RMAImGui(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "Projects", "InputCore", "SlateCore", "Slate", "UMG", "DeveloperSettings", "AIModule" });
        if (Target.bBuildEditor == true) { PublicDependencyModuleNames.AddRange(new string[] { "UnrealEd", "LevelEditor" }); };
        PublicIncludePaths.AddRange(new string[] { Path.Combine(ModuleDirectory, "ThirdParty/ImGui/Source/Public") });
        PrivateIncludePaths.AddRange(new string[] { Path.Combine(ModuleDirectory, "ThirdParty/ImGui/Source/Private") });
        PublicIncludePaths.AddRange(new string[] { Path.Combine(ModuleDirectory, "ThirdParty/ImPlot/Source/Public") });
        PrivateIncludePaths.AddRange(new string[] { Path.Combine(ModuleDirectory, "ThirdParty/ImPlot/Source/Private") });
        PublicIncludePaths.AddRange(new string[] { Path.Combine(ModuleDirectory, "ThirdParty/ImSpinner/Source/Public") });
        PublicDefinitions.Add("WITH_RMAIMGUI");
        PublicDefinitions.Add("IMGUI_DISABLE_OBSOLETE_FUNCTIONS");
        PublicDefinitions.Add("IMGUI_DISABLE_OBSOLETE_KEYIO");
        //Config
        {
            ConfigHierarchy LConfig = ConfigCache.ReadHierarchy(ConfigHierarchyType.Engine, Target.ProjectFile != null ? Target.ProjectFile.Directory : null, Target.Platform);
            //Include Demos
            {
                bool LIncludeDemos = true;
                if (!LConfig.GetBool("/Script/RMAImGui.RMAImGuiSettings", "IncludeDemos", out LIncludeDemos))
                {
                    LIncludeDemos = true;
                }
                PublicDefinitions.Add("WITH_RMAIMGUI_DEMOS=" + (LIncludeDemos ? 1 : 0));
            }
            //Development Only
            {
                bool LDevelopmentOnly = true;
                if (!LConfig.GetBool("/Script/RMAImGui.RMAImGuiSettings", "DevelopmentOnly", out LDevelopmentOnly))
                {
                    LDevelopmentOnly = true;
                }
                PublicDefinitions.Add("RMAIMGUI_DEVELOPMENTONLY=" + (LDevelopmentOnly ? 1 : 0));
            }
        }
    }
}
