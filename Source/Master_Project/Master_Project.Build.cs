// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class Master_Project : ModuleRules
{
	public Master_Project(TargetInfo Target)
	{
		PublicDependencyModuleNames.AddRange(new string[] {
            "Core", "CoreUObject", "Engine",
            "InputCore", "ImageWrapper",
            "OnlineSubsystem", "OnlineSubsystemUtils",
            "Voice" });

        PublicIncludePaths.AddRange(new string[] { "Voice/Public", "Voice/Classes" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		PrivateDependencyModuleNames.Add("OnlineSubsystem");
        DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}
