// Copyright 2015-2017 Piperift. All Rights Reserved.

#include "JinkEditorPrivatePCH.h"
#include "Item.h"

#include "ItemFactory.h"


UItemFactory::UItemFactory(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    SupportedClass = UItem::StaticClass();

    bText = true;
    bCreateNew = true;
    bEditAfterNew = true;
    bEditorImport = true;
}

UObject* UItemFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
    UItem* Item = NewObject<UItem>(InParent, UItem::StaticClass(), Name, Flags);

    /*if (Item != nullptr)
    {
    }*/

    return Item;
}

uint32 UItemFactory::GetMenuCategories() const
{
    if (FJinkCoreModule* JC = FJinkCoreModule::GetInstance()) {
        return JC->GetJCAssetCategoryBit();
    }
    // If JinkCore module is not found use Miscellaneous 
    return EAssetTypeCategories::Misc;
}


