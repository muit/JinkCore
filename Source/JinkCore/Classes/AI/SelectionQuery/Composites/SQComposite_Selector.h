// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "AI/SelectionQuery/SQCompositeNode.h"
#include "SQComposite_Selector.generated.h"

/** 
 * Selector composite node.
 * Selector Nodes execute their children from left to right, and will stop executing its children when one of their children succeeds.
 * If a Selector's child succeeds, the Selector succeeds. If all the Selector's children fail, the Selector fails.
 */
UCLASS()
class JINKCORE_API USQComposite_Selector: public USQCompositeNode
{
	GENERATED_UCLASS_BODY()

	int32 GetNextChildHandler(struct FSelectionQuerySearchData& SearchData, int32 PrevChild) const;

#if WITH_EDITOR
	virtual FName GetNodeIconName() const override;
#endif
};
