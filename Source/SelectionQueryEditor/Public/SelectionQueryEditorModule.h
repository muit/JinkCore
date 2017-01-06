// Copyright 2015-2016 Piperift. All Rights Reserved.
#pragma once

#include "SelectionQueryEditorPrivatePCH.h"
#include "PropertyEditorModule.h"
#include "AIGraphTypes.h"
 
DECLARE_LOG_CATEGORY_EXTERN(LogSelectionQueryEditor, All, All)
 
class FSelectionQueryEditorModule: public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;


    TSharedRef<ISelectionQueryEditor> CreateSelectionQueryEditor(const EToolkitMode::Type Mode, const TSharedPtr< IToolkitHost >& InitToolkitHost, USelectionQuery* Query);

    TSharedPtr<struct FGraphNodeClassHelper> GetClassCache() { return ClassCache; }

private:
	void RegisterPropertyTypeCustomizations();

	/**
	* Registers a custom struct
	*
	* @param StructName				The name of the struct to register for property customization
	* @param StructLayoutDelegate	The delegate to call to get the custom detail layout instance
	*/
	void RegisterCustomPropertyTypeLayout(FName PropertyTypeName, FOnGetPropertyTypeCustomizationInstance PropertyTypeLayoutDelegate);

	void RegisterAssetTypeAction(IAssetTools& AssetTools, TSharedRef<IAssetTypeActions> Action)
	{
		AssetTools.RegisterAssetTypeActions(Action);
		CreatedAssetTypeActions.Add(Action);
	}

	/** All created asset type actions.  Cached here so that we can unregister them during shutdown. */
	TArray< TSharedPtr<IAssetTypeActions> > CreatedAssetTypeActions;

    TSharedPtr<struct FGraphNodeClassHelper> ClassCache;
};