// Copyright 2015-2017 Piperift. All Rights Reserved.

#include "JinkEditorPrivatePCH.h"
#include "AssetTypeAction_SQ.h"
#include "SelectionQuery.h"
#include "JinkCore.h"

#include "AssetToolsModule.h"
#include "ContentBrowserModule.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions"

//////////////////////////////////////////////////////////////////////////
// FAssetTypeAction_LevelInstance

FText FAssetTypeAction_SQ::GetName() const
{
    return LOCTEXT("FAssetTypeAction_SQName", "Selection Query");
}

FColor FAssetTypeAction_SQ::GetTypeColor() const
{
    return FColor(73, 131, 230);
}

UClass* FAssetTypeAction_SQ::GetSupportedClass() const
{
    return USelectionQuery::StaticClass();
}

uint32 FAssetTypeAction_SQ::GetCategories()
{
    if (FJinkCoreModule* JC = FJinkCoreModule::GetInstance()) {
        return JC->GetJCAssetCategoryBit();
    }
    // If JinkCore module is not found use Miscellaneous 
    return EAssetTypeCategories::Misc;
}

void FAssetTypeAction_SQ::GetActions(const TArray<UObject*>& InObjects, FMenuBuilder& MenuBuilder)
{
}

//////////////////////////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE