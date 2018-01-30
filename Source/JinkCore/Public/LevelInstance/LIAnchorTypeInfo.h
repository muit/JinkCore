// Copyright 2015-2018 Piperift. All Rights Reserved.

#pragma once

#include "LIConector.h"
#include "LIAnchorTypeInfo.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType, meta = (DisplayName = "Level Instance Anchor Type Info"))
struct FLIAnchorTypeInfo
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Anchor Type")
    FColor Color;

    UPROPERTY(EditAnywhere, Category = "Anchor Type")
    TSubclassOf<ALIConector> Conector;
    
public:

    UClass* GetConectorType() {
        return Conector ? Conector : ALIConector::StaticClass();
    }


    FLIAnchorTypeInfo() : Color(FColor::Orange)
    {}
};
