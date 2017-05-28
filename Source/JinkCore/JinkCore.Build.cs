// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class JinkCore : ModuleRules
{
    public JinkCore(ReadOnlyTargetRules TargetRules) : base(TargetRules)
    {

        PublicIncludePaths.AddRange(
            new string[] {
                "JinkCore/Public"
				// ... add public include paths required here ...
			}
            );


        PrivateIncludePaths.AddRange(
            new string[] {
                "JinkCore/Private"
				// ... add other private include paths required here ...
			}
            );


        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "AIModule",
                "RHI"
				// ... add other public dependencies that you statically link with here ...
			});

        if (UEBuildConfiguration.bBuildEditor == true)
        {
            PublicDependencyModuleNames.AddRange(
                new string[] {
                    "UnrealEd"
                }
            );
        }
    }
}
