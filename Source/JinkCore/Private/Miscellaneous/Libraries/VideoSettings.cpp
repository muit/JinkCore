// Copyright 2015-2017 Piperift. All Rights Reserved.

#include "JinkCorePrivatePCH.h"
#include "VideoSettings.h"


// Get a list of screen resolutions supported by video adapter
// NOTE: This function needs "RHI" to be added to <Project>.Build.cs file!
bool UVideoSettings::GetSupportedScreenResolutions(TArray<FString>& Resolutions)
{
    FScreenResolutionArray ResolutionsArray;

    if (RHIGetAvailableResolutions(ResolutionsArray, true))  // needs the "RHI" dependency
    {
        for (const FScreenResolutionRHI& Resolution : ResolutionsArray)
        {
            if (Resolution.Width < MIN_SCREEN_WIDTH || Resolution.Height < MIN_SCREEN_HEIGHT)
            {
                continue;
            }

            FString Str = FString::FromInt(Resolution.Width) + "x" + FString::FromInt(Resolution.Height);
            Resolutions.AddUnique(Str);
        }

        return true;
    }

    return false;  // failed to obtain screen resolutions
}


// Get currently set screen resolution
FString UVideoSettings::GetScreenResolution()
{
    UGameUserSettings* Settings = GetGameUserSettings();
    if (!Settings)
    {
        return FString("");
    }

    FIntPoint Resolution = Settings->GetScreenResolution();
    return FString::FromInt(Resolution.X) + "x" + FString::FromInt(Resolution.Y);
}


// Check whether or not we are currently running in fullscreen mode
bool UVideoSettings::IsInFullscreen()
{
    UGameUserSettings* Settings = GetGameUserSettings();
    if (!Settings)
    {
        return false;
    }

    return Settings->GetFullscreenMode() == EWindowMode::Fullscreen;
}


// Set the desired screen resolution (does not change it yet)
bool UVideoSettings::SetScreenResolution(const int32 Width, const int32 Height, const bool Fullscreen)
{
    UGameUserSettings* Settings = GetGameUserSettings();
    if (!Settings)
    {
        return false;
    }

    Settings->SetScreenResolution(FIntPoint(Width, Height));
    Settings->SetFullscreenMode(Fullscreen ? EWindowMode::Fullscreen : EWindowMode::Windowed);
    return true;
}


// Change the current screen resolution
bool UVideoSettings::ChangeScreenResolution(const int32 Width, const int32 Height, const bool Fullscreen)
{
    UGameUserSettings* Settings = GetGameUserSettings();
    if (!Settings)
    {
        return false;
    }

    EWindowMode::Type WindowMode = Fullscreen ? EWindowMode::Fullscreen : EWindowMode::Windowed;
    Settings->RequestResolutionChange(Width, Height, WindowMode, false);
    return true;
}


// Get the current video quality settings
bool UVideoSettings::GetVideoQualitySettings(EAntiAliasing& AntiAliasing, EQuality& Effects, EQuality& PostProcess,
    int32& Resolution, EQuality& Shadow, EQuality& Texture, int32& ViewDistance)
{
    UGameUserSettings* Settings = GetGameUserSettings();
    if (!Settings)
    {
        return false;
    }

    AntiAliasing = static_cast<EAntiAliasing>(Settings->ScalabilityQuality.AntiAliasingQuality);
    Effects = static_cast<EQuality>(Settings->ScalabilityQuality.EffectsQuality);
    PostProcess = static_cast<EQuality>(Settings->ScalabilityQuality.PostProcessQuality);
    Resolution = Settings->ScalabilityQuality.ResolutionQuality;
    Shadow = static_cast<EQuality>(Settings->ScalabilityQuality.ShadowQuality);
    Texture = static_cast<EQuality>(Settings->ScalabilityQuality.TextureQuality);
    ViewDistance = Settings->ScalabilityQuality.ViewDistanceQuality;
    return true;
}


// Set the quality settings (not applied nor saved yet)
bool UVideoSettings::SetVideoQualitySettings(const EAntiAliasing AntiAliasing, const EQuality Effects, const EQuality PostProcess,
    const int32 Resolution, const EQuality Shadow, const EQuality Texture, const int32 ViewDistance)
{
    UGameUserSettings* Settings = GetGameUserSettings();
    if (!Settings)
    {
        return false;
    }

    Settings->ScalabilityQuality.AntiAliasingQuality = (int32)AntiAliasing;
    Settings->ScalabilityQuality.EffectsQuality = (int32)Effects;
    Settings->ScalabilityQuality.PostProcessQuality = (int32)PostProcess;
    Settings->ScalabilityQuality.ResolutionQuality = Resolution;
    Settings->ScalabilityQuality.ShadowQuality = (int32)Shadow;
    Settings->ScalabilityQuality.TextureQuality = (int32)Texture;
    Settings->ScalabilityQuality.ViewDistanceQuality = ViewDistance;
    return true;
}


// Check whether or not we have vertical sync enabled
bool UVideoSettings::IsVSyncEnabled()
{
    UGameUserSettings* Settings = GetGameUserSettings();
    if (!Settings)
    {
        return false;
    }

    return Settings->IsVSyncEnabled();
}


// Set the vertical sync flag
bool UVideoSettings::SetVSyncEnabled(const bool VSync)
{
    UGameUserSettings* Settings = GetGameUserSettings();
    if (!Settings)
    {
        return false;
    }

    Settings->SetVSyncEnabled(VSync);
    return true;
}

// Confirm and save current video mode (resolution and fullscreen/windowed)
bool UVideoSettings::SaveResolutionAndVideoMode()
{
    UGameUserSettings* Settings = GetGameUserSettings();
    if (!Settings)
    {
        return false;
    }

    Settings->ConfirmVideoMode();
    Settings->ApplyResolutionSettings(true);
    Settings->SaveSettings();
    return true;
}

// Confirm and save current quality
bool UVideoSettings::SaveQuality()
{
    UGameUserSettings* Settings = GetGameUserSettings();
    if (!Settings)
    {
        return false;
    }

    Settings->ApplyNonResolutionSettings();
    Settings->SaveSettings();
    return true;
}


// Revert to original video settings
bool UVideoSettings::RevertVideoMode()
{
    UGameUserSettings* Settings = GetGameUserSettings();
    if (!Settings)
    {
        return false;
    }

    Settings->RevertVideoMode();
    return true;
}

FString UVideoSettings::QToString(EQuality Quality)
{
    switch (Quality) {
        case EQuality::Low:      return "Low";
        case EQuality::Medium:   return "Medium";
        case EQuality::High:     return "High";
        case EQuality::VeryHigh: return "Very High";
        default:                 return "Invalid";
    }
}

EQuality UVideoSettings::ToQuality(FString Name) {
    if (Name == "Low")       return EQuality::Low;
    if (Name == "Medium")    return EQuality::Medium;
    if (Name == "High")      return EQuality::High;
    if (Name == "Very High") return EQuality::VeryHigh;

    return EQuality::None;
}

FString UVideoSettings::AAToString(EAntiAliasing Aa)
{
    switch (Aa) {
    case EAntiAliasing::None:       return "No Antialiasing";
    case EAntiAliasing::FXAA:       return "FXAA";
    case EAntiAliasing::TemporalAA: return "Temporal AA";
    default:                        return "FXAA";//If error, return medium antialiasing (FXAA)
    }
}

EAntiAliasing UVideoSettings::ToAntialiasing(FString Name) {
    if (Name == "No Antialiasing") return EAntiAliasing::None;
    if (Name == "FXAA")            return EAntiAliasing::FXAA;
    if (Name == "Temporal AA")     return EAntiAliasing::TemporalAA;

    return EAntiAliasing::FXAA;
}


//---- PRIVATE METHODS -------------------------------------------------------------------------------

// Try to get the GameUserSettings object from the engine
UGameUserSettings* UVideoSettings::GetGameUserSettings()
{
    if (GEngine != nullptr)
    {
        return GEngine->GameUserSettings;
    }
    return nullptr;
}

