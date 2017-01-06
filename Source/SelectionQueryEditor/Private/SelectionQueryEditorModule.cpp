// Copyright 2015-2017 Piperift. All Rights Reserved.

#include "SelectionQueryEditorPrivatePCH.h"

#include "Asset/AssetTypeAction_SelectionQuery.h"
#include "Graph/Nodes/GraphNodePanelFactory_SelectionQuery.h"


DEFINE_LOG_CATEGORY(LogSelectionQueryEditor)
 
#define LOCTEXT_NAMESPACE "SelectionQueryEditor"
 
void FSelectionQueryEditorModule::StartupModule()
{
    UE_LOG(LogSelectionQueryEditor, Warning, TEXT("SelectionQueryEditor: Log Started"));

    FSQEditorThumbnailPool::Create();

    RegisterPropertyTypeCustomizations();

    // Register asset types
    IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
    RegisterAssetTypeAction(AssetTools, MakeShareable(new FAssetTypeAction_SelectionQuery));


    // Register custom graph nodes
    GraphPanelNodeFactory = MakeShareable(new FGraphPanelNodeFactory_SelectionQuery);
    FEdGraphUtilities::RegisterVisualNodeFactory(GraphPanelNodeFactory);
}
 
void FSelectionQueryEditorModule::ShutdownModule()
{
    UE_LOG(LogSelectionQueryEditor, Warning, TEXT("SelectionQueryEditor: Log Ended"));

    ClassCache.Reset();

    // Unregister all the asset types
    if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
    {
        IAssetTools& AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
        for (int32 Index = 0; Index < CreatedAssetTypeActions.Num(); ++Index)
        {
            AssetTools.UnregisterAssetTypeActions(CreatedAssetTypeActions[Index].ToSharedRef());
        }
    }
    CreatedAssetTypeActions.Empty();
}

void FSelectionQueryEditorModule::RegisterPropertyTypeCustomizations()
{
}


void FSelectionQueryEditorModule::RegisterCustomPropertyTypeLayout(FName PropertyTypeName, FOnGetPropertyTypeCustomizationInstance PropertyTypeLayoutDelegate)
{
    check(PropertyTypeName != NAME_None);

    static FName PropertyEditor("PropertyEditor");
    FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>(PropertyEditor);
    PropertyModule.RegisterCustomPropertyTypeLayout(PropertyTypeName, PropertyTypeLayoutDelegate);
}

TSharedRef<ISelectionQueryEditor> FSelectionQueryEditorModule::CreateSelectionQueryEditor(const EToolkitMode::Type Mode, const TSharedPtr< IToolkitHost >& InitToolkitHost, USelectionQuery* Query)
{
    if (!ClassCache.IsValid())
    {
        ClassCache = MakeShareable(new FGraphNodeClassHelper(USQNode::StaticClass()));
        FGraphNodeClassHelper::AddObservedBlueprintClasses(USQComposite_Selector::StaticClass());
        ClassCache->UpdateAvailableBlueprintClasses();
    }

    TSharedRef<FSelectionQueryEditor> NewSelectionQueryEditor(new FSelectionQueryEditor());
    NewSelectionQueryEditor->InitSelectionQueryEditor(Mode, InitToolkitHost, Query);
    return NewSelectionQueryEditor;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_GAME_MODULE(FSelectionQueryEditorModule, SelectionQueryEditor);
