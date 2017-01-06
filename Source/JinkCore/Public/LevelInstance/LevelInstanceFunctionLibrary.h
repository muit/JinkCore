// Copyright 2015-2017 Piperift. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "LIAnchorType.h"
#include "LIAnchorTypeInfo.h"

#include "LevelInstanceFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class JINKCORE_API ULevelInstanceFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_UCLASS_BODY()

public:
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Level Instance|Anchor")
    static bool GetTypeInfo(FLIAnchorType AnchorType, FLIAnchorTypeInfo& Info);
    
    
};
