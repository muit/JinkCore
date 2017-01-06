// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "SQGraphNode_Root.generated.h"

UCLASS()
class USQGraphNode_Root : public USQGraphNode
{
    GENERATED_UCLASS_BODY()

    UPROPERTY(EditAnywhere, Category = Debug)
    TArray<FString> DebugMessages;

    UPROPERTY()
    bool bHasDebugError;

    virtual void AllocateDefaultPins() override;
    virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;

    virtual bool CanDuplicateNode() const override { return false; }
    virtual bool CanUserDeleteNode() const override { return false; }
};
