// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "SelectionQueryGraphNode_Root.generated.h"

UCLASS()
class USelectionQueryGraphNode_Root : public USelectionQueryGraphNode
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(EditAnywhere, Category = Debug)
	TArray<FString> DebugMessages;

	UPROPERTY()
	bool bHasDebugError;

	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	/** gets icon resource name for title bar */
	virtual FName GetNameIcon() const override;

	virtual bool CanDuplicateNode() const override { return false; }
	virtual bool CanUserDeleteNode() const override { return false; }
	virtual bool HasErrors() const override { return false; }
	virtual bool RefreshNodeClass() override { return false; }
};
