// Copyright 2015-2017 Piperift. All Rights Reserved.

#include "SelectionQueryEditorPrivatePCH.h"
#include "SelectionQuery.h"

#include "SelectionQueryFactory.h"

USelectionQueryFactory::USelectionQueryFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SupportedClass = USelectionQuery::StaticClass();
	Formats.Add(TEXT("t3d;Unreal World"));

	bText = true;
	bCreateNew = true;
	bEditorImport = true;
	bEditAfterNew = true;
}

UObject* USelectionQueryFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<USelectionQuery>(InParent, USelectionQuery::StaticClass(), Name, Flags);
}
