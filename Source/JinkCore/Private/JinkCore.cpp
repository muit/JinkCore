// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "JinkCorePrivatePCH.h"

DEFINE_LOG_CATEGORY(JinkCore)

#define LOCTEXT_NAMESPACE "JinkCore"

void FJinkCoreModule::StartupModule()
{
	UE_LOG(JinkCore, Warning, TEXT("JinkCore: Log Started"));

	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	RegisterSettings();
}

void FJinkCoreModule::ShutdownModule()
{
	UE_LOG(JinkCore, Warning, TEXT("JinkCore: Log Ended"));
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	if (UObjectInitialized())
	{
		UnregisterSettings();
	}
}

bool FJinkCoreModule::HandleSettingsSaved()
{
	UJinkCoreSettings* Settings = GetMutableDefault<UJinkCoreSettings>();
	bool ResaveSettings = false;

	if (ModifiedSettingsDelegate.IsBound()) {
		ModifiedSettingsDelegate.Execute();
	}

	// You can put any validation code in here and resave the settings in case an invalid
	// value has been entered
	
	/*Foreach lenguaje*/
	//    Settings->Lenguajes
	//    Check Row Types are correct
	for (FLenguajeData Data : Settings->Lenguajes)
	{
		StaticLoadObject(UObject::StaticClass(), nullptr, *(Data.Audios).ToString());
		UDataTable* DataTable = Cast<UDataTable>(Data.Audios.TryLoad());

		if (DataTable) {
			if (DataTable->RowStruct->GetClass()->GetName() != "LenguajeAudioRow")
				ResaveSettings = true;
		}
	}

	if (ResaveSettings)
	{
		Settings->SaveConfig();
	}

	return true;
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

		SettingsContainer->DescribeCategory("Jink Core",
			LOCTEXT("RuntimeWDCategoryName", "Jink Core"),
			LOCTEXT("RuntimeWDCategoryDescription", "Game configuration for the CustomSettings game module"));

		// Register the settings
		ISettingsSectionPtr SettingsSection = SettingsModule->RegisterSettings("Project", "Jink Core", "General",
			LOCTEXT("RuntimeGeneralSettingsName", "General"),
			LOCTEXT("RuntimeGeneralSettingsDescription", "Base configuration for the Jink core"),
			GetMutableDefault<UJinkCoreSettings>());
		/*
		ISettingsSectionPtr SettingsSection = SettingsModule->RegisterSettings("Project", "Jink Core", "Lenguajes",
			LOCTEXT("RuntimeGeneralSettingsName", "Lenguajes"),
			LOCTEXT("RuntimeGeneralSettingsDescription", "Base configuration for the Jink core"),
			GetMutableDefault<UJinkCoreSettings>());*/

		// Register the save handler to your settings, you might want to use it to
		// validate those or just act to settings changes.
		if (SettingsSection.IsValid())
		{
			SettingsSection->OnModified().BindRaw(this, &FJinkCoreModule::HandleSettingsSaved);
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
		SettingsModule->UnregisterSettings("Project", "Jink Core", "General");
	}
#endif
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FJinkCoreModule, JinkCore)