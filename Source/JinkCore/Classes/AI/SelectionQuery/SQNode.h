// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "SelectionQueryTypes.h"
#include "GameplayTaskOwnerInterface.h"
#include "Tasks/AITask.h"
#include "SQNode.generated.h"

class USelectionQuery;
class USQCompositeNode;

UCLASS(Abstract)
class JINKCORE_API USQNode : public UObject
{
	GENERATED_UCLASS_BODY()

	/** Versioning for updating deprecated properties */
	UPROPERTY()
	int32 VerNum;

	/** node name */
	UPROPERTY(Category = Description, EditAnywhere)
	FString NodeName;

private:
	/** parent node */
	UPROPERTY()
	UBTCompositeNode* ParentNode;

	/** depth in tree */
	uint8 GraphDepth;

public:
	/** @return string containing description of this node with all setup values */
	virtual FString GetStaticDescription() const;

#if WITH_EDITOR

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	/** Get the name of the icon used to display this node in the editor */
	virtual FName GetNodeIconName() const;

	/** Get whether this node is using a blueprint for its logic */
	virtual bool UsesBlueprint() const;
#endif

	virtual void UpdateNodeVersion();

	/** @return name of node */
	FString GetNodeName() const;
};
