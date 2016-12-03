// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SelectionQueryEditor : ModuleRules
{
	public SelectionQueryEditor(TargetInfo Target)
	{
        PrivateIncludePaths.AddRange(
            new string[] {
                "SelectionQueryEditor/Private",
                "Editor/GraphEditor/Private",
                "Editor/GraphEditor/Private/KismetNodes",
                "Editor/GraphEditor/Private/KismetPins",
			}
		);

        PrivateIncludePathModuleNames.AddRange(
            new string[] {
				"AssetRegistry",
				"AssetTools",
                "PropertyEditor"
			}
		);

        PublicDependencyModuleNames.AddRange(
            new string[] {
                "Core",
                "CoreUObject",
                "Engine",
                "Slate",
                "EditorStyle",
                "UnrealEd",
                "KismetWidgets",
                "GraphEditor",
            }
        );

        PrivateDependencyModuleNames.AddRange(
			new string[] {
                "InputCore",
                "SlateCore",
                "RenderCore",
                "PropertyEditor",
                "MessageLog", 
				"AnimGraph",
				"BlueprintGraph",
                "JinkCore",
			}
		);

		PublicIncludePathModuleNames.AddRange(
            new string[] {
                "LevelEditor"
            }
        );

        DynamicallyLoadedModuleNames.AddRange(
            new string[] { 
                "WorkspaceMenuStructure",
            }
		);
	}
}
