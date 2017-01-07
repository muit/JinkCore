// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "SQItemNode.h"
#include "SQItem_Actor.generated.h"

/** 
 * Selector composite node.
 * Selector Nodes execute their children from left to right, and will stop executing its children when one of their children succeeds.
 * If a Selector's child succeeds, the Selector succeeds. If all the Selector's children fail, the Selector fails.
 */
UCLASS()
class SELECTIONQUERY_API USQItem_Actor: public USQItemNode
{
    GENERATED_UCLASS_BODY()

public:
    UPROPERTY(EditAnywhere, Category = "Item")
    TSubclassOf<AActor> Item;

    virtual UObject* GetThumbnailAssetObject() override;
};
