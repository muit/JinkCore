// Copyright 2015-2016 Piperift. All Rights Reserved.
// Original Library by https://github.com/ImpetusGames/UE4-Settings

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "VideoSettings.generated.h"

/** Used by FPostProcessSettings Anti-aliasings */
UENUM(BlueprintType)
enum class EAntiAliasing : uint8
{
    None UMETA(DisplayName = "None"),
    FXAA UMETA(DisplayName = "FXAA"),
    TemporalAA UMETA(DisplayName = "TemporalAA"),
    MAX,
};

UENUM(BlueprintType)
enum class EQuality : uint8
{
    Low      UMETA(DisplayName = "Low"),
    Medium   UMETA(DisplayName = "Medium"),
    High     UMETA(DisplayName = "High"),
    VeryHigh UMETA(DisplayName = "Very High"),
    None
};

UENUM(BlueprintType)
enum class EViewDistance : uint8
{
    Low    UMETA(DisplayName = "Low"),
    Medium UMETA(DisplayName = "Medium"),
    High   UMETA(DisplayName = "High")
};


#define MIN_SCREEN_WIDTH 1024
#define MIN_SCREEN_HEIGHT 768

/**
 * Custom Game Settings Function Library.
 */
UCLASS()
class JINKCORE_API UVideoSettings : public UBlueprintFunctionLibrary
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
    static bool SetVideoQualitySettings(const EAntiAliasing AntiAliasing = EAntiAliasing::TemporalAA, const EQuality Effects = EQuality::High, const EQuality PostProcess = EQuality::High,
        const int32 Resolution = 100, const EQuality Shadow = EQuality::High, const EQuality Texture = EQuality::High, const int32 ViewDistance = 3);

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

    // Enum Handlers
    UFUNCTION(BlueprintPure, Category = "Video Settings")
    static FString QToString(const EQuality Quality);
    UFUNCTION(BlueprintPure, Category = "Video Settings")
    static EQuality ToQuality(const FString Name);
    
    UFUNCTION(BlueprintPure, Category = "Video Settings")
    static FString AAToString(const EAntiAliasing Aa);
    UFUNCTION(BlueprintPure, Category = "Video Settings")
    static EAntiAliasing ToAntialiasing(const FString Name);


private:
    // Try to get the GameUserSettings object from the engine
    static UGameUserSettings* GetGameUserSettings();

};
