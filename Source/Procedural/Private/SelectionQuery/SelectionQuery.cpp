// Copyright 2015-2018 Piperift. All Rights Reserved.

#include "ProceduralPrivatePCH.h"
#include "SelectionQuery.h"

#define LOCTEXT_NAMESPACE "USelectionQuery"

USelectionQuery::USelectionQuery(const FObjectInitializer & ObjectInitializer)
    : Super(ObjectInitializer)
{
    QueryName = "Node";
}

FSQExecution USelectionQuery::Execute_Implementation() {
    return FSQExecution();
}

void USelectionQuery::Selector(FSQExecution& Result, TArray<FSQExecution> SubNodes, float Weight)
{
    //Calculate total Weight
    float TotalWeight = 0;
    for (auto& Node : SubNodes) {
        if(Node.IsValid)
            TotalWeight += Node.Weight;
    }

    //Set Default result
    Result = FSQExecution();

    if (TotalWeight <= 0)
        return;

    TSet<TSubclassOf<UItem>> ItemRecord;

    float RandomWeight = FMath::RandRange(0.0f, TotalWeight);

    //Find the Selected Subnode
    for (auto& Node : SubNodes) {
        if (Node.IsValid) {
            RandomWeight -= Node.Weight;

            if (RandomWeight > 0) {
                //Registry unselected Items
                ItemRecord.Append(Node.ItemRecord);
            }
            else {
                //If it's selected
                Result = Node;
                Result.Weight = Weight;

                //Registry all the other items
                Result.AddItemsToRecord(ItemRecord);
                break;
            }
        }
    }
}

FSQExecution USelectionQuery::Item(TSubclassOf<UItem> Item, float Weight)
{
    return FSQExecution(Item, Weight);
}

//////////////////////////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE
