// Copyright 2015-2017 Piperift. All Rights Reserved.

#pragma once
#include "SQGraphNode_Item.generated.h"


UCLASS()
class USQGraphNode_Item : public USQGraphNode
{
    GENERATED_UCLASS_BODY()

    virtual void AllocateDefaultPins() override;
    virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
    /** Gets a list of actions that can be done to this particular node */
    virtual void GetContextMenuActions(const FGraphNodeContextMenuBuilder& Context) const override;

    //virtual bool CanPlaceBreakpoints() const override { return true; }

    virtual UObject* GetThumbnailAssetObject();
};
