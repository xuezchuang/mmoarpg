//Copyright (C) RenZhai.2022.All Rights Reserved.

using UnrealBuildTool;

public class SimpleCombat : ModuleRules
{
	public SimpleCombat(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"GameplayTags",
				"USDSchemas",
				"GameplayCameras",
				"GameplayAbilities",
				"Niagara"
				// ... add other public dependencies that you statically link with here ...
			});
		
			if (Target.bBuildEditor == true)
			{
				PrivateDependencyModuleNames.AddRange(
				new string[]
				{
					"Persona"
				});
			}

			PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"UMG"
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
