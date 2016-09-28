// Copyright 2015-2016 Piperift. All Rights Reserved.

#pragma once
 
#include "Engine.h"
#include "ModuleManager.h"
#include "UnrealEd.h"
 
DECLARE_LOG_CATEGORY_EXTERN(JinkEditor, All, All)
 
class FJinkEditorModule: public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
 
};