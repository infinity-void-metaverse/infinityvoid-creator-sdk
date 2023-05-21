// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class ZEZ : ModuleRules
{
	public ZEZ(ReadOnlyTargetRules Target) : base(Target)
	{
		bEnableExceptions  = true;

		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Json", "HTTP", "Engine", "InputCore","ALSV4_CPP", "UMG", "EnhancedInput"});

		PrivateDependencyModuleNames.AddRange(new string[] { "ReadyPlayerMe" });

		PrivateDependencyModuleNames.AddRange(new string[] { "WebBrowserWidget", "WebBrowser", "UMG" });
		
	}
}
