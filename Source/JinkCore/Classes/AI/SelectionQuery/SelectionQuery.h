// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "SQCompositeNode.h"
#include "SQTaskNode.h"
#include "Engine/Blueprint.h"
#include "SelectionQuery.generated.h"

class USQCompositeNode;
class USQDecorator;

UCLASS(BlueprintType)
class JINKCORE_API USelectionQuery : public UObject
{
	GENERATED_UCLASS_BODY()

	/** root node of loaded tree */
	UPROPERTY()
	USQCompositeNode* RootNode;

#if WITH_EDITORONLY_DATA

	/** Graph for Behavior Tree */
	UPROPERTY()
	class UEdGraph*	SQGraph;

	/** Info about the graphs we last edited */
	UPROPERTY()
	TArray<FEditedDocumentInfo> LastEditedDocuments;

#endif

	/** root level decorators, used by subtrees */
	UPROPERTY()
	TArray<USQDecorator*> RootDecorators;

	/** logic operators for root level decorators, used by subtrees  */
	UPROPERTY()
	TArray<FSQDecoratorLogic> RootDecoratorOps;

	/** memory size required for instance of this tree */
	uint16 InstanceMemorySize;
};
