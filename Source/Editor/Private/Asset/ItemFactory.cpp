// Copyright 2015-2018 Piperift. All Rights Reserved.

#include "JinkEditorPrivatePCH.h"
#include "EditorUtilityBlueprint.h"
#include "Kismet2/KismetEditorUtilities.h"


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
    return FKismetEditorUtilities::CreateBlueprint(Class, InParent, Name, BPTYPE_Normal, UBlueprint::StaticClass(), UBlueprintGeneratedClass::StaticClass());
    //return NewObject<UItem>(InParent, UItem::StaticClass(), Name, Flags);
}
