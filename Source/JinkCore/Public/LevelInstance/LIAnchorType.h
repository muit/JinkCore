// Copyright 2015-2018 Piperift. All Rights Reserved.

#pragma once

#include "Object.h"
#include "LIAnchorType.generated.h"

#define ANCHOR_None FString("None")

class UJCGenerationSettings;
struct FLIAnchorTypeInfo;

/**
 * 
 */
USTRUCT(BlueprintType)
struct JINKCORE_API FLIAnchorType
{
    GENERATED_USTRUCT_BODY()

    FLIAnchorType() : Id(0), Name(ANCHOR_None)
    {}

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anchor Type")
    uint8 Id;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anchor Type")
    FString Name;


    FORCEINLINE bool operator==(const FLIAnchorType& Other) const {
        return Other.Name == Name;
        //TODO: Fix Id assignation in editor.
        //return Other.Id == Id;
    }

    FORCEINLINE bool operator!=(const FLIAnchorType& Other) const {
        return !Other.Id == Id;
    }

    bool GetAnchorInfo(FLIAnchorTypeInfo& Info);
};
