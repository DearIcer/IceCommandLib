// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class IceDevelopmentAssetBatching : ModuleRules
{
	public IceDevelopmentAssetBatching(ReadOnlyTargetRules Target) : base(Target)
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
				System.IO.Path.GetFullPath(Target.RelativeEnginePath) + "/Source/Editor/Blutility/Private",
				System.IO.Path.GetFullPath(Target.RelativeEnginePath) + "Developer/AssetTools/Private"
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"AssetTools",
				"Blutility",
				"Core", 
				"CoreUObject", 
				"Engine", 
				"EditorScriptingUtilities",
				"InputCore" ,
				"PhysicsCore",
				"UMG",
				"UMGEditor",
				"UnrealEd",
				"ContentBrowser",
				"LevelEditor"
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Projects",
				"InputCore",
				"UnrealEd",
				"ToolMenus",
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore", 
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
