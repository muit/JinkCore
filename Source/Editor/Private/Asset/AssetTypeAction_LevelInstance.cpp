// Copyright 2015-2018 Piperift. All Rights Reserved.

#include "JinkEditorPrivatePCH.h"
#include "AssetTypeAction_LevelInstance.h"
#include "LevelInstance/LevelInstance.h"
#include "JinkCore.h"

#include "AssetToolsModule.h"
#include "ContentBrowserModule.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions"

//////////////////////////////////////////////////////////////////////////
// FAssetTypeAction_LevelInstance

FText FAssetTypeAction_LevelInstance::GetName() const
{
    return LOCTEXT("FAssetTypeAction_LevelInstanceName", "Level Instance");
}

FColor FAssetTypeAction_LevelInstance::GetTypeColor() const
{
    return FColor(244, 116, 66);
}

UClass* FAssetTypeAction_LevelInstance::GetSupportedClass() const
{
    return ULevelInstance::StaticClass();
}

uint32 FAssetTypeAction_LevelInstance::GetCategories()
{
    if (FJinkCoreModule* JC = FJinkCoreModule::GetInstance()) {
        return JC->GetJCAssetCategoryBit();
    }
    // If JinkCore module is not found use Miscellaneous 
    return EAssetTypeCategories::Misc;
}

void FAssetTypeAction_LevelInstance::GetActions(const TArray<UObject*>& InObjects, FMenuBuilder& MenuBuilder)
{
}

//////////////////////////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE