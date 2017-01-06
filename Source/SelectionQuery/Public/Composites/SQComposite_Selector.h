// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "SQCompositeNode.h"
#include "SQComposite_Selector.generated.h"

class USQItemNode;

/** 
 * Selector composite node.
 * Selector Nodes execute their children from left to right, and will stop executing its children when one of their children succeeds.
 * If a Selector's child succeeds, the Selector succeeds. If all the Selector's children fail, the Selector fails.
 */
UCLASS()
class SELECTIONQUERY_API USQComposite_Selector: public USQCompositeNode
{
    GENERATED_UCLASS_BODY()

#if WITH_EDITOR
    virtual FName GetNodeIconName() const override;
#endif


public:
    UPROPERTY(EditAnywhere, Category = "Selector")
    float SelectorClass;

    virtual USQItemNode* Run() override;
};
