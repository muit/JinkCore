// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "SelectionQueryEditorPrivatePCH.h"
#include "SelectionQueryFactory.h"
#include "SelectionQueryEditorModule.h"
#include "SelectionQuery/SelectionQuery.h"

USelectionQueryFactory::USelectionQueryFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SupportedClass = USelectionQuery::StaticClass();
	bEditAfterNew = true;
	bCreateNew = true;
}

UObject* USelectionQueryFactory::FactoryCreateNew(UClass* Class,UObject* InParent,FName Name,EObjectFlags Flags,UObject* Context,FFeedbackContext* Warn)
{
	check(Class->IsChildOf(USelectionQuery::StaticClass()));
	return NewObject<USelectionQuery>(InParent, Class, Name, Flags);
}

bool USelectionQueryFactory::CanCreateNew() const
{
	if (GetDefault<UEditorExperimentalSettings>()->bEQSEditor)
	{
		return true;
	}

	// Check ini to see if we should enable creation
	bool bEnableSelectionQueryEd = false;
	GConfig->GetBool(TEXT("SelectionQueryEd"), TEXT("EnableSelectionQueryEd"), bEnableSelectionQueryEd, GEngineIni);
	return bEnableSelectionQueryEd;
}
