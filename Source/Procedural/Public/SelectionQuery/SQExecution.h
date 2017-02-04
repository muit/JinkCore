// Copyright 2015-2017 Piperift. All Rights Reserved.

#pragma once

#include "Item.h"
#include "SQExecution.generated.h"

/**
 * 
 */
USTRUCT()
struct PROCEDURAL_API FSQExecution
{
public:
    GENERATED_USTRUCT_BODY()

    FSQExecution() : Weight(1), IsValid(false) {}

    FSQExecution(TSubclassOf<UItem> Item, float InWeight) :
        ItemValue(Item),
        Weight(InWeight),
        IsValid(true) 
    {
        AddItemToRecord(Item);
    }

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Node")
    bool IsValid;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Node")
    float Weight;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Node")
    TSubclassOf<UItem> ItemValue;

    TSet<TSubclassOf<UItem>> ItemRecord;

    void AddItemToRecord(TSubclassOf<UItem> Item) {
        ItemRecord.Add(Item);
    }

    void AddItemsToRecord(TSet<TSubclassOf<UItem>> ItemSet) {
        ItemRecord.Append(ItemSet);
    }
};
