// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class JinkEditor : ModuleRules
{
	public JinkEditor(ReadOnlyTargetRules TargetRules) : base(TargetRules)
	{
		
		PublicIncludePaths.AddRange(
			new string[] {
                "Editor/Public"
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				"Editor/Private"
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
                "InputCore"
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
                "Core",
				"CoreUObject",
				"Engine",
                "UnrealEd",
                "Blutility",
				"Slate",
				"SlateCore",
                "AssetTools",
                "EditorStyle",
                "PropertyEditor",
                "DetailCustomizations",
                "ContentBrowser",
                "Settings",
                "JinkCore",
                "Procedural"
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
