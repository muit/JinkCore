// Copyright 2015-2017 Piperift. All Rights Reserved.

#include "JinkEditorPrivatePCH.h"
#include "AssetTypeAction_Buff.h"
#include "Buff.h"
#include "JinkCore.h"

#include "AssetToolsModule.h"
#include "ContentBrowserModule.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions"

//////////////////////////////////////////////////////////////////////////
// FAssetTypeAction_LevelInstance

FText FAssetTypeAction_Buff::GetName() const
{
    return LOCTEXT("FAssetTypeAction_BuffName", "Buff");
}

FColor FAssetTypeAction_Buff::GetTypeColor() const
{
    return FColor(73, 131, 230);
}

UClass* FAssetTypeAction_Buff::GetSupportedClass() const
{
    return UBuff::StaticClass();
}

uint32 FAssetTypeAction_Buff::GetCategories()
{
    if (FJinkCoreModule* JC = FJinkCoreModule::GetInstance()) {
        return JC->GetJCAssetCategoryBit();
    }
    // If JinkCore module is not found use Miscellaneous 
    return EAssetTypeCategories::Misc;
}

void FAssetTypeAction_Buff::GetActions(const TArray<UObject*>& InObjects, FMenuBuilder& MenuBuilder)
{
}

//////////////////////////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE