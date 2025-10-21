// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MMOARPG : ModuleRules
{
	public MMOARPG(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core", 
			"CoreUObject",
			"Engine",
			"GameplayAbilities",
			"AIModule",
			"PhysicsCore",
			"Niagara",
			"GameplayAbilities",
			"GameplayTags",
			"GameplayTasks",
			"Sockets",
			"Networking",
			"NavigationSystem",

			"MMOARPGCommon",
			"InputCore",
			"HeadMountedDisplay",
			"EnhancedInput",
			"SimpleThread",
			"SimpleNetChannel",
			
			"Slate",
			"SlateCore",
			"Landscape"
			});

		PublicIncludePaths.AddRange(new string[]{
			
			//"CoreUObject/Public/UObject"
			//"B2Network/Public",
			//"MyProject1/Text",
			//"MyProject1/Test",
			});

		PrivateIncludePaths.AddRange(new string[]{
			
			"MMOARPG",
			"MMOARPG/Data",
			"MMOARPG/Core/Game/Character",
			"MMOARPG/Core",
			"MMOARPG/Core/Game",
			"MMOARPG/UI/Core",
			"MMOARPG/UI/Common",
			"MMOARPG/UI/Game",
			"MMOARPG/Core/Game/Character",
			"MMOARPG/Core/Game/Animation/Instance/Core",
			//"MMOARPG",
			//"MMOARPG",
			//"MMOARPG",
			//"MMOARPG",
			//"MMOARPG",
			//"MMOARPG",
			//"MMOARPG",
			//"MMOARPG",
			//"MMOARPG",

		});
	}
}
