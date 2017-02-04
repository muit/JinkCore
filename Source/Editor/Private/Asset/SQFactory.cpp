// Copyright 2015-2017 Piperift. All Rights Reserved.

#include "JinkEditorPrivatePCH.h"
#include "EditorUtilityBlueprint.h"
#include "Kismet2/KismetEditorUtilities.h"


#include "SelectionQuery.h"

#include "SQFactory.h"


USQFactory::USQFactory(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    SupportedClass = USelectionQuery::StaticClass();

    bText = true;
    bCreateNew = true;
    bEditAfterNew = true;
    bEditorImport = true;
}

UObject* USQFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
    return FKismetEditorUtilities::CreateBlueprint(Class, InParent, Name, BPTYPE_Normal, UBlueprint::StaticClass(), UBlueprintGeneratedClass::StaticClass());
    //return NewObject<USelectionQuery>(InParent, USelectionQuery::StaticClass(), Name, Flags);
}
