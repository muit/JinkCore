// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "JinkCorePrivatePCH.h"

// Settings
#include "JinkCoreSettings.h"

#include "ISettingsModule.h"
#include "ISettingsSection.h"
#include "ISettingsContainer.h"


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