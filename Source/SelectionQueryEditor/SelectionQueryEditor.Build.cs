// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SelectionQueryEditor : ModuleRules
{
    public SelectionQueryEditor(TargetInfo Target)
    {
        
        PublicIncludePaths.AddRange(
            new string[] {
                "SelectionQueryEditor/Public"
                // ... add public include paths required here ...
            }
            );
                
        
        PrivateIncludePaths.AddRange(
            new string[] {
                "SelectionQueryEditor/Private",
                "Editor/GraphEditor/Private",
                "Editor/GraphEditor/Private/KismetNodes",
                "Editor/GraphEditor/Private/KismetPins"
                // ... add other private include paths required here ...
            }
            );

        PrivateIncludePathModuleNames.AddRange(
            new string[] {
                "Settings",
                "AssetTools",
            }
        );

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "Slate",
                "EditorStyle",
                "UnrealEd",
                "KismetWidgets",
                "GraphEditor",
                "Kismet"
                // ... add other public dependencies that you statically link with here ...
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "InputCore",
                "SlateCore",
                "RenderCore",
                "PropertyEditor",
                "WorkspaceMenuStructure",
                "EditorStyle",
                "ContentBrowser",
                "SelectionQuery",
                "JinkCore"
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
