// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "AI/SelectionQuery/SelectionQueryTypes.h"
#include "AI/SelectionQuery/SQNode.h"
#include "SQCompositeNode.generated.h"

class USQNode;
class USQItemNode;
class USQCompositeNode;

USTRUCT()
struct FSQCompositeChild
{
	GENERATED_USTRUCT_BODY()

public:
	/** child node */
	UPROPERTY()
	USQCompositeNode* ChildComposite;

	UPROPERTY()
	USQItemNode* ChildItem;

    bool ChildIsComposite();
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


	virtual void GenerateItems(FSelectionQueryInstance& QueryInstance) const { checkNoEntry(); }

	virtual void PostLoad() override;
	void UpdateNodeVersion() override;
};


//////////////////////////////////////////////////////////////////////////
// Inlines

FORCEINLINE bool FSQCompositeChild::ChildIsComposite() { 
    return ChildComposite;
}

FORCEINLINE USQNode* USQCompositeNode::GetChildNode(int32 Index) const
{
    if (Children.IsValidIndex(Index))
    {
        FSQCompositeChild Child = Children[Index];
        return Child.ChildIsComposite() ? Child.ChildComposite : Child.ChildTask;
    }
    return nullptr;
}

FORCEINLINE int32 USQCompositeNode::GetChildrenNum() const
{
	return Children.Num();
}
