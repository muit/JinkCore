// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "SQGraphNode_Composite.generated.h"

UCLASS()
class USQGraphNode_Composite : public USQGraphNode
{
    GENERATED_UCLASS_BODY()

    virtual void AllocateDefaultPins() override;
    virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;

    virtual void GetContextMenuActions(const FGraphNodeContextMenuBuilder& Context) const override;
};
