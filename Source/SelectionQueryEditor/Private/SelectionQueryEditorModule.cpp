// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "SelectionQueryEditorPrivatePCH.h"

#include "DetailCustomizations/SelDirectionCustomization.h"
#include "DetailCustomizations/SelTraceDataCustomization.h"
#include "DetailCustomizations/SelQueryTestDetails.h"

#include "ModuleManager.h"
#include "Toolkits/ToolkitManager.h"
#include "SGraphNode_SelectionQuery.h"
#include "EdGraphUtilities.h"

#include "SelectionQuery/Generators/SelQueryGenerator_BlueprintBase.h"


IMPLEMENT_MODULE( FSelectionQueryEditorModule, SelectionQueryEditor );
DEFINE_LOG_CATEGORY(LogSelectionQueryEditor);

const FName FSelectionQueryEditorModule::SelectionQueryEditorAppIdentifier( TEXT( "SelectionQueryEditorApp" ) );

class FGraphPanelNodeFactory_SelectionQuery : public FGraphPanelNodeFactory
{
	virtual TSharedPtr<class SGraphNode> CreateNode(UEdGraphNode* Node) const override
	{
		if (USelectionQueryGraphNode* SelQueryNode = Cast<USelectionQueryGraphNode>(Node))
		{
			return SNew(SGraphNode_SelectionQuery, SelQueryNode);
		}

		return NULL;
	}
};

TSharedPtr<FGraphPanelNodeFactory> GraphPanelNodeFactory_SelectionQuery;

void FSelectionQueryEditorModule::StartupModule()
{
	MenuExtensibilityManager = MakeShareable(new FExtensibilityManager);
	ToolBarExtensibilityManager = MakeShareable(new FExtensibilityManager);

	GraphPanelNodeFactory_SelectionQuery = MakeShareable( new FGraphPanelNodeFactory_SelectionQuery() );
	FEdGraphUtilities::RegisterVisualNodeFactory(GraphPanelNodeFactory_SelectionQuery);

	ItemDataAssetTypeActions = MakeShareable(new FAssetTypeActions_SelectionQuery);
	FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get().RegisterAssetTypeActions(ItemDataAssetTypeActions.ToSharedRef());

	// Register the details customizer
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.RegisterCustomPropertyTypeLayout( "SelDirection", FOnGetPropertyTypeCustomizationInstance::CreateStatic( &FSelDirectionCustomization::MakeInstance ) );
	PropertyModule.RegisterCustomPropertyTypeLayout( "SelTraceData", FOnGetPropertyTypeCustomizationInstance::CreateStatic( &FSelTraceDataCustomization::MakeInstance ) );
	PropertyModule.RegisterCustomClassLayout( "SelQueryTest", FOnGetDetailCustomizationInstance::CreateStatic( &FSelQueryTestDetails::MakeInstance ) );
	PropertyModule.NotifyCustomizationModuleChanged();
}


void FSelectionQueryEditorModule::ShutdownModule()
{
	if (!UObjectInitialized())
	{
		return;
	}

	ClassCache.Reset();
	MenuExtensibilityManager.Reset();
	ToolBarExtensibilityManager.Reset();

	if ( GraphPanelNodeFactory_SelectionQuery.IsValid() )
	{
		FEdGraphUtilities::UnregisterVisualNodeFactory(GraphPanelNodeFactory_SelectionQuery);
		GraphPanelNodeFactory_SelectionQuery.Reset();
	}

	// Unregister the SelectionQuery item data asset type actions
	if (ItemDataAssetTypeActions.IsValid())
	{
		if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
		{
			FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get().UnregisterAssetTypeActions(ItemDataAssetTypeActions.ToSharedRef());
		}
		ItemDataAssetTypeActions.Reset();
	}

	// Unregister the details customization
	if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
	{
		FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
		PropertyModule.UnregisterCustomPropertyTypeLayout( "SelDirection" );
		PropertyModule.UnregisterCustomPropertyTypeLayout( "SelTraceData" );

		PropertyModule.UnregisterCustomClassLayout( "SelQueryTest" );
		PropertyModule.NotifyCustomizationModuleChanged();
	}
}


TSharedRef<ISelectionQueryEditor> FSelectionQueryEditorModule::CreateSelectionQueryEditor( const EToolkitMode::Type Mode, const TSharedPtr< IToolkitHost >& InitToolkitHost, USelectionQuery* Query)
{
	if (!ClassCache.IsValid())
	{
		ClassCache = MakeShareable(new FGraphNodeClassHelper(USelQueryNode::StaticClass()));
		FGraphNodeClassHelper::AddObservedBlueprintClasses(USelQueryGenerator_BlueprintBase::StaticClass());
		ClassCache->UpdateAvailableBlueprintClasses();
	}

	TSharedRef< FSelectionQueryEditor > NewSelectionQueryEditor( new FSelectionQueryEditor() );
	NewSelectionQueryEditor->InitSelectionQueryEditor(Mode, InitToolkitHost, Query);
	return NewSelectionQueryEditor;
}
