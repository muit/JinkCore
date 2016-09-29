// Copyright 2015-2016 Piperift. All Rights Reserved.
#pragma once

#include "JinkCorePrivatePCH.h"

// Settings
#include "JinkCoreSettings.h"

#include "ISettingsModule.h"
#include "ISettingsSection.h"
#include "ISettingsContainer.h"

DECLARE_LOG_CATEGORY_EXTERN(JinkCore, All, All)

class FJinkCoreModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	virtual bool SupportsDynamicReloading() override { return true; }

private:

	// Callback for when the settings were saved.
	bool HandleSettingsSaved();

	void RegisterSettings();

	void UnregisterSettings();
};