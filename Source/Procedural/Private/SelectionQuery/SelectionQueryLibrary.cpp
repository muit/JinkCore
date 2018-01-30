// Copyright 2015-2018 Piperift. All Rights Reserved.

#include "ProceduralPrivatePCH.h"
#include "SelectionQueryLibrary.h"

void USelectionQueryLibrary::RunQuery(TSubclassOf<USelectionQuery> Query, bool& Success, TSubclassOf<UItem>& Item)
{
    //Create the Query
    USelectionQuery* QueryObj = NewObject<USelectionQuery>((UObject*)GetTransientPackage(), Query);

    FSQExecution Result = QueryObj->Execute();
    
    Success = Result.IsValid;
    Item =  Result.ItemValue;
}

void USelectionQueryLibrary::GetAllQueryItems(TSubclassOf<USelectionQuery> Query, bool& Success, TArray<TSubclassOf<UItem>>& Items)
{
    //Create the Query
    USelectionQuery* QueryObj = NewObject<USelectionQuery>((UObject*)GetTransientPackage(), Query);

    FSQExecution Result = QueryObj->Execute();

    Success = Result.IsValid;
    Items = Result.ItemRecord.Array();
}

UClass* USelectionQueryLibrary::RunClassPT(FPTClassExecution& Node)
{
    return Node.ItemValue;
}

void USelectionQueryLibrary::PTCSelector(FPTClassExecution & Result, TArray<FPTClassExecution> SubNodes, float Weight)
{
    //Calculate total Weight
    float TotalWeight = 0;
    for (auto& Node : SubNodes) {
        if (Node.IsValid)
            TotalWeight += Node.Weight;
    }

    //Set Default result
    Result = FPTClassExecution();

    if (TotalWeight <= 0)
        return;

    TSet<UClass*> ItemRecord;

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

/*TAssetPtr<UObject>& USelectionQueryLibrary::RunAssetPT()
{
    return TAssetPtr<UObject>();
}*/



TAssetPtr<UDataAsset> USelectionQueryLibrary::RunDataAssetPT(FPTDataAssetExecution& Node)
{
    return Node.ItemValue;
}

FPTClassExecution USelectionQueryLibrary::PTCItem(UClass * Item, float Weight)
{
    return FPTClassExecution(Item, Weight);
}

void USelectionQueryLibrary::PTDASelector(FPTDataAssetExecution & Result, TArray<FPTDataAssetExecution> SubNodes, float Weight)
{
    //Calculate total Weight
    float TotalWeight = 0;
    for (auto& Node : SubNodes) {
        if (Node.IsValid)
            TotalWeight += Node.Weight;
    }

    //Set Default result
    Result = FPTDataAssetExecution();

    if (TotalWeight <= 0)
        return;

    TSet<TAssetPtr<UDataAsset>> ItemRecord;

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

FPTDataAssetExecution USelectionQueryLibrary::PTDAItem(TAssetPtr<UDataAsset> Item, float Weight)
{
    return FPTDataAssetExecution(Item, Weight);
}
