// Copyright 2015-2017 Piperift. All Rights Reserved.

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
