// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "SelectionQueryGraphNode_Composite.generated.h"

UCLASS()
class USelectionQueryGraphNode_Composite : public USelectionQueryGraphNode
{
	GENERATED_UCLASS_BODY()

	virtual void AllocateDefaultPins() override;
	virtual void PostPlacedNewNode() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;

	virtual void GetContextMenuActions(const FGraphNodeContextMenuBuilder& Context) const override;
	void CreateAddTestSubMenu(class FMenuBuilder& MenuBuilder, UEdGraph* Graph) const;

	void CalculateWeights();
	void UpdateNodeData();
};
