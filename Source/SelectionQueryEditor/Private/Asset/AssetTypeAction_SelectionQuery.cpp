// Copyright 2015-2017 Piperift. All Rights Reserved.

#include "SelectionQueryEditorPrivatePCH.h"
#include "AssetTypeAction_SelectionQuery.h"
#include "SelectionQuery.h"
#include "JinkCore.h"

#include "AssetToolsModule.h"
#include "ContentBrowserModule.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions"

//////////////////////////////////////////////////////////////////////////
// FAssetTypeAction_SelectionQuery

FText FAssetTypeAction_SelectionQuery::GetName() const
{
	return LOCTEXT("FAssetTypeAction_SelectionQueryName", "Selection Query");
}

FColor FAssetTypeAction_SelectionQuery::GetTypeColor() const
{
	return FColor(50, 255, 190);
}

UClass* FAssetTypeAction_SelectionQuery::GetSupportedClass() const
{
	return USelectionQuery::StaticClass();
}

void FAssetTypeAction_SelectionQuery::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor)
{
    const EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;

    for (auto Object : InObjects)
    {
        USelectionQuery* Query = Cast<USelectionQuery>(Object);
        if (Query) {
            TSharedRef<FSelectionQueryEditor> NewSelectionQueryEditor(new FSelectionQueryEditor());
            NewSelectionQueryEditor->InitSelectionQueryEditor(Mode, EditWithinLevelEditor, Query);
        }
    }
}

uint32 FAssetTypeAction_SelectionQuery::GetCategories()
{
	if (FJinkCoreModule* JC = FJinkCoreModule::GetInstance()) {
		return JC->GetJCAssetCategoryBit();
	}
	// If JinkCore module is not found use Miscellaneous 
	return EAssetTypeCategories::Misc;
}

void FAssetTypeAction_SelectionQuery::GetActions(const TArray<UObject*>& InObjects, FMenuBuilder& MenuBuilder)
{
}

//////////////////////////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE