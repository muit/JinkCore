// Copyright 2015-2016 Piperift. All Rights Reserved.
#pragma once

#include "JinkCorePrivatePCH.h"

// Settings
#include "JinkCoreSettings.h"

#if WITH_EDITOR
#include "ISettingsModule.h"
#include "ISettingsSection.h"
#include "ISettingsContainer.h"
#endif //WITH_EDITOR

DECLARE_LOG_CATEGORY_EXTERN(JinkCore, All, All);

class USelectionQueryManager;

class FJinkCoreModule : public IModuleInterface
{
public:

    /**
    * Singleton-like access to this module's interface.
    * Beware of calling this during the shutdown phase, though.  Your module might have been unloaded already.
    *
    * @return Returns singleton instance, loading the module on demand if needed
    */
    FORCEINLINE static FJinkCoreModule& Get()
    {
        return FModuleManager::LoadModuleChecked< FJinkCoreModule >("JinkCore");
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
};