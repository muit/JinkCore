// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "JinkCorePrivatePCH.h"

#include "LIAnchorType.h"

// Settings
#include "JCGeneralSettings.h"
#include "JCGenerationSettings.h"

DEFINE_LOG_CATEGORY(LogJinkCore)

#define LOCTEXT_NAMESPACE "JinkCore"

void FJinkCoreModule::StartupModule()
{
    UE_LOG(LogJinkCore, Warning, TEXT("JinkCore: Log Started"));

    // This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
    
#if WITH_EDITOR 
    if (GIsEditor)
    {
        // Register asset types
        IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
        // register JK category so that assets can register to it
        JCAssetCategoryBit = AssetTools.RegisterAdvancedAssetCategory(FName(TEXT("JC")), LOCTEXT("JCAssetCategory", "Jink Core"));
    }
#endif // WITH_EDITOR 

    RegisterSettings();
}

void FJinkCoreModule::ShutdownModule()
{
    UE_LOG(LogJinkCore, Warning, TEXT("JinkCore: Log Ended"));
    // This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
    // we call this function before unloading the module.

    if (UObjectInitialized())
    {
        UnregisterSettings();
    }
}

void FJinkCoreModule::RegisterSettings()
{
#if WITH_EDITOR
    // Registering some settings is just a matter of exposing the default UObject of
    // your desired class, feel free to add here all those settings you want to expose
    // to your LDs or artists.

    if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
    {
        // Create the new category
        ISettingsContainerPtr SettingsContainer = SettingsModule->GetContainer("Project");

        SettingsContainer->DescribeCategory("JinkCore",
            LOCTEXT("RuntimeWDCategoryName", "Jink Core"),
            LOCTEXT("RuntimeWDCategoryDescription", "Game configuration for the CustomSettings game module"));

        // Register the General settings
        ISettingsSectionPtr GeneralSettingsSection = SettingsModule->RegisterSettings("Project", "Game", "General",
            LOCTEXT("RuntimeGeneralSettingsName", "General"),
            LOCTEXT("RuntimeGeneralSettingsDescription", "Base configuration for the Jink core"),
            GetMutableDefault<UJCGeneralSettings>());

        ISettingsSectionPtr GenerationSettingsSection = SettingsModule->RegisterSettings("Project", "Game", "Generation",
            LOCTEXT("FlipbookEditorSettingsName", "Generation"),
            LOCTEXT("FlipbookEditorSettingsDescription", "Configuration for the generation functionality."),
            GetMutableDefault<UJCGenerationSettings>());

        // Register the save handler to your settings, you might want to use it to
        // validate those or just act to settings changes.
        if (GeneralSettingsSection.IsValid())
        {
            GeneralSettingsSection->OnModified().BindRaw(this, &FJinkCoreModule::HandleGeneralSettingsSaved);
        }
        if (GenerationSettingsSection.IsValid())
        {
            GenerationSettingsSection->OnModified().BindRaw(this, &FJinkCoreModule::HandleGenerationSettingsSaved);
        }
    }
#endif
}

void FJinkCoreModule::UnregisterSettings()
{
#if WITH_EDITOR
    // Ensure to unregister all of your registered settings here, hot-reload would
    // otherwise yield unexpected results.

    if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
    {
        SettingsModule->UnregisterSettings("Project", "Game", "General");
        SettingsModule->UnregisterSettings("Project", "Game", "Generation");
    }
#endif
}

bool FJinkCoreModule::HandleGeneralSettingsSaved()
{
    UJCGeneralSettings* Settings = GetMutableDefault<UJCGeneralSettings>();
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

    return true;
}

bool FJinkCoreModule::HandleGenerationSettingsSaved()
{
    UJCGenerationSettings* Settings = GetMutableDefault<UJCGenerationSettings>();
    bool ResaveSettings = false;

    // You can put any validation code in here and resave the settings in case an invalid
    // value has been entered

    TArray<FString> Keys;
    Settings->AnchorTypes.GetKeys(Keys);

    if (!Settings->AnchorTypes.Contains(ANCHOR_None) || 
        Keys.IndexOfByKey(ANCHOR_None) != 0)
    {
        //Add None Type if needed
        Settings->AnchorTypes.FindOrAdd(ANCHOR_None);

        //Sort map to put None the first
        Settings->AnchorTypes.KeySort([](FString A, FString B) {
            return A.Equals(ANCHOR_None); // sort keys depending on None
        });

        ResaveSettings = true;
    }


    if (ModifiedGenerationSettingsDelegate.IsBound()) {
        ModifiedGenerationSettingsDelegate.Execute();
    }

    if (ResaveSettings)
    {
        Settings->SaveConfig();
    }
    return true;
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FJinkCoreModule, JinkCore)