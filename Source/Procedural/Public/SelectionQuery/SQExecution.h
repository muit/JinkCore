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

    UPROPERTY(Transient)
    TSet<TSubclassOf<UItem>> ItemRecord;

    void AddItemToRecord(TSubclassOf<UItem> Item) {
        ItemRecord.Add(Item);
    }

    void AddItemsToRecord(TSet<TSubclassOf<UItem>> ItemSet) {
        ItemRecord.Append(ItemSet);
    }
};

USTRUCT()
struct PROCEDURAL_API FPTClassExecution
{
public:
    GENERATED_USTRUCT_BODY()

    FPTClassExecution() : Weight(1), IsValid(false) {}

    FPTClassExecution(UClass* Item, float InWeight) :
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
    UClass* ItemValue;

    UPROPERTY(Transient)
    TSet<UClass*> ItemRecord;

    void AddItemToRecord(UClass* Item) {
        ItemRecord.Add(Item);
    }

    void AddItemsToRecord(TSet<UClass*> ItemSet) {
        ItemRecord.Append(ItemSet);
    }
};

USTRUCT()
struct PROCEDURAL_API FPTDataAssetExecution
{
public:
    GENERATED_USTRUCT_BODY()

    FPTDataAssetExecution() : Weight(1), IsValid(false) {}

    FPTDataAssetExecution(TAssetSubclassOf<UDataAsset> Item, float InWeight) :
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
    TAssetSubclassOf<UDataAsset> ItemValue;

    UPROPERTY(Transient)
    TSet<TAssetSubclassOf<UDataAsset>> ItemRecord;

    void AddItemToRecord(TAssetSubclassOf<UDataAsset> Item) {
        ItemRecord.Add(Item);
    }

    void AddItemsToRecord(const TSet<TAssetSubclassOf<UDataAsset>> ItemSet) {
        ItemRecord.Append(ItemSet);
    }
};
