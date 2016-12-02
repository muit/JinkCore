// Copyright 2015-2017 Piperift. All Rights Reserved.

#include "JinkEditorPrivatePCH.h"
#include "IBlutilityModule.h"
#include "EditorUtilityBlueprint.h"
#include "Kismet2/KismetEditorUtilities.h"

#include "ComplexItem.h"

#include "ComplexItemFactory.h"

UComplexItemFactory::UComplexItemFactory(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    SupportedClass = UComplexItem::StaticClass();

    bText = true;
    bCreateNew = true;
    bEditAfterNew = true;
    bEditorImport = true;
}

UObject* UComplexItemFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
    return FKismetEditorUtilities::CreateBlueprint(Class, InParent, Name, BPTYPE_Normal, UBlueprint::StaticClass(), UBlueprintGeneratedClass::StaticClass());
}

uint32 UComplexItemFactory::GetMenuCategories() const
{
    if (FJinkCoreModule* JC = FJinkCoreModule::GetInstance()) {
        return JC->GetJCAssetCategoryBit();
    }
    // If JinkCore module is not found use Miscellaneous 
    return EAssetTypeCategories::Misc;
}





