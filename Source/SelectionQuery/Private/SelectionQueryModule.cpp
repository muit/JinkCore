// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "SelectionQueryPrivatePCH.h"

DEFINE_LOG_CATEGORY(LogSelectionQuery)

#define LOCTEXT_NAMESPACE "SelectionQueryModule"

void FSelectionQueryModule::StartupModule()
{
    UE_LOG(LogSelectionQuery, Warning, TEXT("SelectionQuery: Log Started"));

    // This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
    
    RegisterSettings();
}

void FSelectionQueryModule::ShutdownModule()
{
    UE_LOG(LogSelectionQuery, Warning, TEXT("SelectionQuery: Log Ended"));
    // This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
    // we call this function before unloading the module.

    if (UObjectInitialized())
    {
        UnregisterSettings();
    }
}

void FSelectionQueryModule::RegisterSettings()
{
#if WITH_EDITOR
    // Registering some settings is just a matter of exposing the default UObject of
    // your desired class, feel free to add here all those settings you want to expose
    // to your LDs or artists.

    if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
    {
        // Get Project Settings category
        ISettingsContainerPtr SettingsContainer = SettingsModule->GetContainer("Project");

        SettingsContainer->DescribeCategory("SelectionQuery",
            LOCTEXT("RuntimeWDCategoryName", "Jink Core"),
            LOCTEXT("RuntimeWDCategoryDescription", "Game configuration for the CustomSettings game module"));

        // Register the General settings
        /*ISettingsSectionPtr GeneralSettingsSection = SettingsModule->RegisterSettings("Project", "Game", "General",
            LOCTEXT("RuntimeGeneralSettingsName", "General"),
            LOCTEXT("RuntimeGeneralSettingsDescription", "Base configuration for the Jink core"),
            GetMutableDefault<UJCGeneralSettings>());*/

        // Register the save handler to your settings, you might want to use it to
        // validate those or just act to settings changes.
        /*if (GeneralSettingsSection.IsValid())
        {
            GeneralSettingsSection->OnModified().BindRaw(this, &FSelectionQueryModule::HandleGeneralSettingsSaved);
        }*/
    }
#endif
}

void FSelectionQueryModule::UnregisterSettings()
{
#if WITH_EDITOR
    // Ensure to unregister all of your registered settings here, hot-reload would
    // otherwise yield unexpected results.

    if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
    {
        //SettingsModule->UnregisterSettings("Project", "Game", "General");
    }
#endif
}

bool FSelectionQueryModule::HandleGeneralSettingsSaved()
{
    /*UJCGeneralSettings* Settings = GetMutableDefault<UJCGeneralSettings>();
    bool ResaveSettings = false;

    if (ModifiedGeneralSettingsDelegate.IsBound()) {
        ModifiedGeneralSettingsDelegate.Execute();
    }

    // You can put any validation code in here and resave the settings in case an invalid
    // value has been entered

    if (ResaveSettings)
    {
        Settings->SaveConfig();
    }
    */
    return true;
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FSelectionQueryModule, SelectionQuery)