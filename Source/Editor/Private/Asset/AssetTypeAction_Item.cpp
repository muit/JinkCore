// Copyright 2015-2017 Piperift. All Rights Reserved.

#include "JinkEditorPrivatePCH.h"
#include "AssetTypeAction_Item.h"
#include "Item.h"
#include "JinkCore.h"

#include "AssetToolsModule.h"
#include "ContentBrowserModule.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions"

//////////////////////////////////////////////////////////////////////////
// FAssetTypeAction_LevelInstance

FText FAssetTypeAction_Item::GetName() const
{
	return LOCTEXT("FAssetTypeAction_ItemName", "Item");
}

FColor FAssetTypeAction_Item::GetTypeColor() const
{
	return FColor(73, 131, 230);
}

UClass* FAssetTypeAction_Item::GetSupportedClass() const
{
	return UItem::StaticClass();
}

uint32 FAssetTypeAction_Item::GetCategories()
{
	if (FJinkCoreModule* JC = FJinkCoreModule::GetInstance()) {
		return JC->GetJCAssetCategoryBit();
	}
	// If JinkCore module is not found use Miscellaneous 
	return EAssetTypeCategories::Misc;
}

void FAssetTypeAction_Item::GetActions(const TArray<UObject*>& InObjects, FMenuBuilder& MenuBuilder)
{
}

//////////////////////////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE