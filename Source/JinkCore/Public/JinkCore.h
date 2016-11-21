// Copyright 2015-2016 Piperift. All Rights Reserved.
#pragma once

#include "Private/JinkCorePrivatePCH.h"

// Settings
#include "JinkCoreSettings.h"

#if WITH_EDITOR
#include "ISettingsModule.h"
#include "ISettingsSection.h"
#include "ISettingsContainer.h"

#include "Developer/AssetTools/Public/IAssetTools.h"
#include "Developer/AssetTools/Public/AssetToolsModule.h"
#endif //WITH_EDITOR

DECLARE_LOG_CATEGORY_EXTERN(JinkCore, All, All);

class FJinkCoreModule : public IModuleInterface
{
public:

	// Get Jink Core module instance
	FORCEINLINE static FJinkCoreModule* GetInstance() { 
		return &FModuleManager::LoadModuleChecked<FJinkCoreModule>("JinkCore");
	}

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	virtual bool SupportsDynamicReloading() override { return true; }

	DECLARE_DELEGATE_RetVal(void, FOnModifiedSettings)
	FOnModifiedSettings& OnModifiedSettings()
	{
		return ModifiedSettingsDelegate;
	}

private:
	/** Holds a delegate that is executed after the settings section has been modified. */
	FOnModifiedSettings ModifiedSettingsDelegate;

	// Callback for when the settings were saved.
	bool HandleSettingsSaved();
	void RegisterSettings();
	void UnregisterSettings();

#if WITH_EDITOR
private:
	EAssetTypeCategories::Type JCAssetCategoryBit;
public:
	EAssetTypeCategories::Type GetJCAssetCategoryBit() const { return JCAssetCategoryBit; }
#endif

};