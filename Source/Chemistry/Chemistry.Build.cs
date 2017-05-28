// Copyright 2015-2017 Piperift. All Rights Reserved.

using UnrealBuildTool;

public class Chemistry : ModuleRules
{
	public Chemistry(ReadOnlyTargetRules TargetRules) : base(TargetRules)
    {
		
		PublicIncludePaths.AddRange(
			new string[] {
                "Chemistry/Public"
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				"Chemistry/Private"
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
                "JinkCore"
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
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
