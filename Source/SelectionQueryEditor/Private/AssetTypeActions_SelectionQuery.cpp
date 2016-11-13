// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "SelectionQueryEditorPrivatePCH.h"
#include "Toolkits/AssetEditorManager.h"

#include "Editor/SelectionQueryEditor/Public/SelectionQueryEditorModule.h"
#include "Editor/SelectionQueryEditor/Public/ISelectionQueryEditor.h"

#include "SelectionQuery/SelectionQuery.h"
#include "AssetTypeActions_SelectionQuery.h"

#include "AIModule.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions"

void FAssetTypeActions_SelectionQuery::OpenAssetEditor( const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor )
{
	EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;

	for (auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		auto Script = Cast<USelQuery>(*ObjIt);
		if (Script != NULL)
		{
			FSelectionQueryEditorModule& SelectionQueryEditorModule = FModuleManager::LoadModuleChecked<FSelectionQueryEditorModule>( "SelectionQueryEditor" );
			TSharedRef< ISelectionQueryEditor > NewEditor = SelectionQueryEditorModule.CreateSelectionQueryEditor( EToolkitMode::Standalone, EditWithinLevelEditor, Script );
		}
	}
}

UClass* FAssetTypeActions_SelectionQuery::GetSupportedClass() const
{ 
	return USelQuery::StaticClass(); 
}

uint32 FAssetTypeActions_SelectionQuery::GetCategories()
{
	IAIModule& AIModule = FModuleManager::GetModuleChecked<IAIModule>("AIModule").Get();
	return AIModule.GetAIAssetCategoryBit();
}
#undef LOCTEXT_NAMESPACE
