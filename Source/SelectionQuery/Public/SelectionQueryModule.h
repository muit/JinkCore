// Copyright 2015-2016 Piperift. All Rights Reserved.
#pragma once

#include "Private/SelectionQueryPrivatePCH.h"

#if WITH_EDITOR
#include "ISettingsModule.h"
#include "ISettingsSection.h"
#include "ISettingsContainer.h"

#include "Developer/AssetTools/Public/IAssetTools.h"
#include "Developer/AssetTools/Public/AssetToolsModule.h"
#endif //WITH_EDITOR

DECLARE_LOG_CATEGORY_EXTERN(LogSelectionQuery, All, All);

class FSelectionQueryModule : public IModuleInterface
{
public:

	// Get Jink Core module instance
	FORCEINLINE static FSelectionQueryModule* GetInstance() { 
		return &FModuleManager::LoadModuleChecked<FSelectionQueryModule>("SelectionQuery");
	}

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	virtual bool SupportsDynamicReloading() override { return true; }

	DECLARE_DELEGATE_RetVal(void, FOnModifiedSettings)
	FOnModifiedSettings& OnModifiedGeneralSettings()
	{
		return ModifiedGeneralSettingsDelegate;
	}

    DECLARE_DELEGATE_RetVal(void, FOnModifiedSettings)
    FOnModifiedSettings& OnModifiedGenerationSettings()
    {
        return ModifiedGenerationSettingsDelegate;
    }

private:
	/** Holds a delegate that is executed after the settings section has been modified. */
	FOnModifiedSettings ModifiedGeneralSettingsDelegate;
    /** Holds a delegate that is executed after the settings section has been modified. */
    FOnModifiedSettings ModifiedGenerationSettingsDelegate;

	void RegisterSettings();
	void UnregisterSettings();

    // Callbacks for when the settings were saved.
    bool HandleGeneralSettingsSaved();

#if WITH_EDITOR
private:
	EAssetTypeCategories::Type JCAssetCategoryBit;
public:
	EAssetTypeCategories::Type GetJCAssetCategoryBit() const { return JCAssetCategoryBit; }
#endif

};