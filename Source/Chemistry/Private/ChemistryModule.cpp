// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "ChemistryPrivatePCH.h"

// Settings
#include "ChemistrySettings.h"

DEFINE_LOG_CATEGORY(LogChemistry)

#define LOCTEXT_NAMESPACE "ChemistryModule"

void FChemistryModule::StartupModule()
{
    UE_LOG(LogChemistry, Warning, TEXT("Chemistry: Log Started"));

    // This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
    
    RegisterSettings();
}

void FChemistryModule::ShutdownModule()
{
    UE_LOG(LogChemistry, Warning, TEXT("Chemistry: Log Ended"));
    // This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
    // we call this function before unloading the module.

    if (UObjectInitialized())
    {
        UnregisterSettings();
    }
}

void FChemistryModule::RegisterSettings()
{
#if WITH_EDITOR
    // Registering some settings is just a matter of exposing the default UObject of
    // your desired class, feel free to add here all those settings you want to expose
    // to your LDs or artists.

    if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
    {
        // Get Project Settings category
        ISettingsContainerPtr SettingsContainer = SettingsModule->GetContainer("Project");

        // Register Chemistry settings
        ISettingsSectionPtr SettingsSection = SettingsModule->RegisterSettings("Project", "Game", "Chemistry",
            LOCTEXT("RuntimeChemistrySettingsName", "Chemistry"),
            LOCTEXT("RuntimeChemistryDescription", "Chemistry configuration of Jink core."),
            GetMutableDefault<UChemistrySettings>());

        // Register the save handler to your settings, you might want to use it to
        // validate those or just act to settings changes.
        if (SettingsSection.IsValid())
        {
            SettingsSection->OnModified().BindRaw(this, &FChemistryModule::HandleSettingsSaved);
        }
    }
#endif
}

void FChemistryModule::UnregisterSettings()
{
#if WITH_EDITOR
    // Ensure to unregister all of your registered settings here, hot-reload would
    // otherwise yield unexpected results.

    if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
    {
        SettingsModule->UnregisterSettings("Project", "Game", "Chemistry");
    }
#endif
}

bool FChemistryModule::HandleSettingsSaved()
{
    UChemistrySettings* Settings = GetMutableDefault<UChemistrySettings>();
    bool ResaveSettings = false;

    if (ModifiedSettingsDelegate.IsBound()) {
        ModifiedSettingsDelegate.Execute();
    }

    // You can put any validation code in here and resave the settings in case an invalid
    // value has been entered

    if (ResaveSettings)
    {
        Settings->SaveConfig();
    }
    
    return true;
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FChemistryModule, Chemistry)