// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "SelectionQuery/SelectionQueryTypes.h"
#include "SelectionQuery/SQNode.h"
#include "SQCompositeNode.generated.h"

class USQNode;
class USQTaskNode;
class USQCompositeNode;

USTRUCT()
struct FSQCompositeChild
{
	GENERATED_USTRUCT_BODY()

	/** child node */
	UPROPERTY()
	USQCompositeNode* ChildComposite;

	UPROPERTY()
	USQTaskNode* ChildTask;
};

UCLASS(Abstract)
class JINKCORE_API USQCompositeNode : public USQNode
{
	GENERATED_UCLASS_BODY()

	/** child nodes */
	UPROPERTY()
	TArray<FSQCompositeChild> Children;


	/** @return child node at given index */
	USQNode* GetChildNode(int32 Index) const;

	/** @return children count */
	int32 GetChildrenNum() const;


	virtual void GenerateItems(FEnvQueryInstance& QueryInstance) const { checkNoEntry(); }

	virtual void PostLoad() override;
	void UpdateNodeVersion() override;
};};


//////////////////////////////////////////////////////////////////////////
// Inlines

FORCEINLINE USQNode* USQCompositeNode::GetChildNode(int32 Index) const
{
	return Children.IsValidIndex(Index) ?
		(Children[Index].ChildComposite ?
			(USQNode*)Children[Index].ChildComposite :
			(USQNode*)Children[Index].ChildTask) :
		nullptr;
}

FORCEINLINE int32 USQCompositeNode::GetChildrenNum() const
{
	return Children.Num();
}
