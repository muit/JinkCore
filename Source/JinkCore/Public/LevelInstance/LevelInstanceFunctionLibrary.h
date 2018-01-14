// Copyright 2015-2018 Piperift. All Rights Reserved.

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
    static bool GetTypeInfo(UPARAM(ref) FLIAnchorType& Type, FLIAnchorTypeInfo& Info);

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Level Instance|Anchor")
    static bool Equals(UPARAM(ref) const FLIAnchorType & TypeA, UPARAM(ref) const FLIAnchorType & TypeB);
    
    
};
