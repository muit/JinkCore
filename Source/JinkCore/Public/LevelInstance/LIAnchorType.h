// Copyright 2015-2016 Piperift. All Rights Reserved.

#pragma once

#include "Object.h"
#include "LIAnchorType.generated.h"

#define ANCHOR_None FString("None")

/**
 * 
 */
USTRUCT(BlueprintType)
struct JINKCORE_API FLIAnchorType
{
	GENERATED_USTRUCT_BODY()

	FLIAnchorType() : Name(ANCHOR_None) 
	{}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anchor Type")
	FString Name;


	FORCEINLINE bool operator==(const FLIAnchorType& Other) const {
		return Other.Name.Equals(Name);
	}

	FORCEINLINE bool operator!=(const FLIAnchorType& Other) const {
		return !Other.Name.Equals(Name);
	}
};
