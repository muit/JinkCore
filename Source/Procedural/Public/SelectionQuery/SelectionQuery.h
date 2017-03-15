// Copyright 2015-2017 Piperift. All Rights Reserved.

#pragma once

#include "SQExecution.h"
#include "SelectionQuery.generated.h"

/**
*
*/
UCLASS(Abstract, Blueprintable)
class PROCEDURAL_API USelectionQuery : public UObject
{
    GENERATED_BODY()

public:
    USelectionQuery(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName QueryName;

    UFUNCTION(BlueprintNativeEvent)
    FSQExecution Execute();

    UFUNCTION(BlueprintCallable, BlueprintPure, Category ="Selection Query")
    void Selector(FSQExecution& Result, TArray<FSQExecution> SubNodes, float Weight = 1);

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Selection Query")
    FSQExecution Item(TSubclassOf<UItem> Item, float Weight = 1);
};
