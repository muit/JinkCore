// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "EdGraph/EdGraphNode.h"
#include "SelectionQueryGraphNode.generated.h"

UCLASS()
class USelectionQueryGraphNode : public UEdGraphNode
{
	GENERATED_UCLASS_BODY()

	virtual class USelectionQueryGraph* GetSelectionQueryGraph();

	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;

    virtual bool CanCreateUnderSpecifiedSchema(const UEdGraphSchema* DesiredSchema) const override;

public:
    virtual void PostEditChangeProperty(struct FPropertyChangedEvent& e);
    virtual UEdGraphPin* GetInputPin() const { return nullptr; }
    virtual UEdGraphPin* GetOutputPin() const { return nullptr; }
    virtual void NodeConnectionListChanged() override;

};
