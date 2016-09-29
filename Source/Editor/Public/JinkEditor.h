// Copyright 2015-2016 Piperift. All Rights Reserved.
#pragma once

#include "JinkEditorPrivatePCH.h"

#include "PropertyEditorModule.h"
 
DECLARE_LOG_CATEGORY_EXTERN(JinkEditor, All, All)
 
class FJinkEditorModule: public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	void RegisterPropertyTypeCustomizations();

	/**
	* Registers a custom struct
	*
	* @param StructName				The name of the struct to register for property customization
	* @param StructLayoutDelegate	The delegate to call to get the custom detail layout instance
	*/
	void RegisterCustomPropertyTypeLayout(FName PropertyTypeName, FOnGetPropertyTypeCustomizationInstance PropertyTypeLayoutDelegate);
};