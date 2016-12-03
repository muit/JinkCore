// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "SQNode.h"
#include "SQItemNode.generated.h"

/** 
 * Items are leaf nodes of selection queries, which return an actual item
 */

UCLASS(Abstract)
class JINKCORE_API USQItemNode : public USQNode
{
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category=Option)
	FString OptionName;

	/** type of generated items */
	UPROPERTY()
	TSubclassOf<UObject> ItemValue;

	virtual FString GetItem() const { return TEXT("None"); }

	virtual void PostLoad() override;
};
