// Copyright 2015-2016 Piperift. All Rights Reserved.
// Original Library by https://github.com/ImpetusGames/UE4-Settings

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "VideoSettingsLib.generated.h"

/** Used by FPostProcessSettings Anti-aliasings */
UENUM(BlueprintType)
enum class EAntiAliasing : uint8
{
	AA_None UMETA(DisplayName = "None"),
	AA_FXAA UMETA(DisplayName = "FXAA"),
	AA_TemporalAA UMETA(DisplayName = "TemporalAA"),
	AA_MAX,
};

UENUM(BlueprintType)
enum class EQuality : uint8
{
	Q_Low      UMETA(DisplayName = "Low"),
	Q_Medium   UMETA(DisplayName = "Medium"),
	Q_High     UMETA(DisplayName = "High"),
	Q_VeryHigh UMETA(DisplayName = "Very High")
};

UENUM(BlueprintType)
enum class EViewDistance : uint8
{
	VD_Low    UMETA(DisplayName = "Low"),
	VD_Medium UMETA(DisplayName = "Medium"),
	VD_High   UMETA(DisplayName = "High")
};


#define MIN_SCREEN_WIDTH 1024
#define MIN_SCREEN_HEIGHT 768

/**
 * Custom Game Settings Function Library.
 */
UCLASS()
class JINKCORE_API UVideoSettingsLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	// Get a list of screen resolutions supported on this machine
	UFUNCTION(BlueprintPure, Category = "Video Settings")
	static bool GetSupportedScreenResolutions(TArray<FString>& Resolutions);

	// Get currently set screen resolution
	UFUNCTION(BlueprintPure, Category = "Video Settings")
	static FString GetScreenResolution();

	// Check whether or not we are currently running in fullscreen mode
	UFUNCTION(BlueprintPure, Category = "Video Settings")
	static bool IsInFullscreen();

	// Set the desired screen resolution (does not change it yet)
	UFUNCTION(BlueprintCallable, Category = "Video Settings")
	static bool SetScreenResolution(const int32 Width, const int32 Height, const bool Fullscreen);

	// Change the current screen resolution
	UFUNCTION(BlueprintCallable, Category = "Video Settings")
	static bool ChangeScreenResolution(const int32 Width, const int32 Height, const bool Fullscreen);

	// Get the current video quality settings
	UFUNCTION(BlueprintCallable, Category = "Video Settings")
	static bool GetVideoQualitySettings(EAntiAliasing& AntiAliasing, EQuality& Effects, EQuality& PostProcess, int32& Resolution, EQuality& Shadow, EQuality& Texture, int32& ViewDistance);

	// Set the quality settings (not applied nor saved yet)
	UFUNCTION(BlueprintCallable, Category = "Video Settings")
	static bool SetVideoQualitySettings(const EAntiAliasing AntiAliasing = EAntiAliasing::AA_TemporalAA, const EQuality Effects = EQuality::Q_High, const EQuality PostProcess = EQuality::Q_High,
		const int32 Resolution = 100, const EQuality Shadow = EQuality::Q_High, const EQuality Texture = EQuality::Q_High, const int32 ViewDistance = 3);

	// Check whether or not we have vertical sync enabled
	UFUNCTION(BlueprintPure, Category = "Video Settings")
	static bool IsVSyncEnabled();

	// Set the vertical sync flag
	UFUNCTION(BlueprintCallable, Category = "Video Settings")
	static bool SetVSyncEnabled(const bool VSync);

	// Confirm and save current video mode (resolution and fullscreen/windowed) as well as quality settings
	UFUNCTION(BlueprintCallable, Category = "Video Settings")
	static bool SaveVideoModeAndQuality();

	// Revert to original video settings
	UFUNCTION(BlueprintCallable, Category = "Video Settings")
	static bool RevertVideoMode();

private:
	// Try to get the GameUserSettings object from the engine
	static UGameUserSettings* GetGameUserSettings();

};
