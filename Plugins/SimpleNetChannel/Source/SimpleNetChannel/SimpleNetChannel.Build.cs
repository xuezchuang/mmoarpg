// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class SimpleNetChannel : ModuleRules
{
	public SimpleNetChannel(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				"SimpleNetChannel",
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Sockets",
				// ... add private dependencies that you statically link with here ...	
			}
			);

		//如果是在项目里面想用到该内容 就把它打开
		//PublicDefinitions.Add("PLATFORM_PROJECT");
		if (PublicDefinitions.Contains("PLATFORM_PROJECT"))
		{
			PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Engine",
				"Sockets",
				"Networking"
			});
		}

		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);

		//bUsePrecompiled = true;
		//PrecompileForTargets = PrecompileTargetsType.Any;
	}
}
