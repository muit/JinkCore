// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "JinkCorePrivatePCH.h"
#include "AI/SelectionQuery/Composites/SQComposite_Selector.h"

USQComposite_Selector::USQComposite_Selector(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	NodeName = "Selector";

	OnNextChild.BindUObject(this, &USQComposite_Selector::GetNextChildHandler);
}

int32 USQComposite_Selector::GetNextChildHandler(FBehaviorTreeSearchData& SearchData, int32 PrevChild) const
{
	// success = quit
	int32 NextChildIdx = SQSpecialChild::ReturnToParent;

	if (PrevChild == BTSpecialChild::NotInitialized)
	{
		// newly activated: start from first
		NextChildIdx = 0;
	}
	else if (LastResult == ESQNodeResult::Failed && (PrevChild + 1) < GetChildrenNum())
	{
		// failed = choose next child
		NextChildIdx = PrevChild + 1;
	}

	return NextChildIdx;
}

#if WITH_EDITOR

FName USQComposite_Selector::GetNodeIconName() const
{
	return FName("BTEditor.Graph.BTNode.Composite.Selector.Icon");
}

#endif