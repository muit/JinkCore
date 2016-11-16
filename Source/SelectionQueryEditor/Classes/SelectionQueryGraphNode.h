// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "AIGraphNode.h"
#include "SelectionQueryGraphNode.generated.h"

UCLASS()
class USelectionQueryGraphNode : public UAIGraphNode
{
	GENERATED_UCLASS_BODY()

	virtual class USelectionQueryGraph* GetSelectionQueryGraph();

	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FText GetDescription() const override;

    /** gets icon resource name for title bar */
    virtual FName GetNameIcon() const;

	virtual bool CanCreateUnderSpecifiedSchema(const UEdGraphSchema* DesiredSchema) const override;
};
