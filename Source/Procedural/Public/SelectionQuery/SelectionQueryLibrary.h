// Copyright 2015-2017 Piperift. All Rights Reserved.

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


    UFUNCTION(BlueprintCallable, Category = ProbabilityTree)
    static UClass* RunClassPT(FPTClassExecution& Node);

    //UFUNCTION(BlueprintCallable, Category = ProbabilityTree)
    //static TAssetPtr<UObject>& RunAssetPT();


    UFUNCTION(BlueprintCallable, Category = ProbabilityTree)
    static TAssetSubclassOf<UDataAsset> RunDataAssetPT(FPTDataAssetExecution& Node);
};
