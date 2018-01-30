// Copyright 2015-2018 Piperift. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Engine/DataAsset.h"
#include "Item.h"
#include "SelectionQuery.h"
#include "SelectionQueryLibrary.generated.h"

/**
 * 
 */
UCLASS()
class PROCEDURAL_API USelectionQueryLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

    UFUNCTION(BlueprintCallable, Category = "Selection Query")
    static void RunQuery(TSubclassOf<USelectionQuery> Query, bool& Success, TSubclassOf<UItem>& Item);

    UFUNCTION(BlueprintCallable, Category = "Selection Query")
    static void GetAllQueryItems(TSubclassOf<USelectionQuery> Query, bool& Success, TArray<TSubclassOf<UItem>>& Item);


    //~ Begin Class Probability Tree Interface
    UFUNCTION(BlueprintCallable, Category = "Probability Tree|Class")
    static UClass* RunClassPT(UPARAM(ref) FPTClassExecution& Node);

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Probability Tree|Class", meta = (DisplayName = "Selector"))
    static void PTCSelector(FPTClassExecution& Result, TArray<FPTClassExecution> SubNodes, float Weight = 1);

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Probability Tree|Class", meta = (DisplayName = "Item"))
    static FPTClassExecution PTCItem(UClass* Item, float Weight = 1);
    //~ End Class Probability Tree Interface

    //~ Begin Asset Probability Tree Interface
    //UFUNCTION(BlueprintCallable, Category = ProbabilityTree)
    //static TAssetPtr<UObject>& RunAssetPT();
    //~ End Asset Probability Tree Interface


    //~ Begin DataAsset Probability Tree Interface
    UFUNCTION(BlueprintCallable, Category = "Probability Tree|Data Asset")
    static TAssetPtr<UDataAsset> RunDataAssetPT(UPARAM(ref) FPTDataAssetExecution& Node);

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Probability Tree|Data Asset", meta = (DisplayName = "Selector"))
    static void PTDASelector(FPTDataAssetExecution& Result, TArray<FPTDataAssetExecution> SubNodes, float Weight = 1);

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Probability Tree|Data Asset", meta = (DisplayName = "Item"))
    static FPTDataAssetExecution PTDAItem(TAssetPtr<UDataAsset> Item, float Weight = 1);
    //~ End DataAsset Probability Tree Interface
};
