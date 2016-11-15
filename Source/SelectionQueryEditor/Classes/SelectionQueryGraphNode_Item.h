// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "SelectionQueryGraphNode_Task.generated.h"


UCLASS()
class USelectionQueryGraphNode_Task : public USelectionQueryGraphNode
{
	GENERATED_UCLASS_BODY()

	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	/** Gets a list of actions that can be done to this particular node */
	virtual void GetContextMenuActions(const FGraphNodeContextMenuBuilder& Context) const override;

	virtual bool CanPlaceBreakpoints() const override { return true; }
};
