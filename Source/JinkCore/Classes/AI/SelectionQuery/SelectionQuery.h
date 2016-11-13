// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "SelectionQueryTypes.h"
#include "Engine/DataAsset.h"
#include "SelectionQuery.generated.h"

#if WITH_EDITORONLY_DATA
class UEdGraph;
#endif // WITH_EDITORONLY_DATA


UCLASS(BlueprintType)
class JINKCORE_API USelectionQuery : public UDataAsset
{
	GENERATED_UCLASS_BODY()

	/** root node of loaded tree */
	UPROPERTY()
	USQCompositeNode* RootNode;

#if WITH_EDITORONLY_DATA
	/** Graph for query */
	UPROPERTY()
	UEdGraph*	EdGraph;
#endif

protected:
	friend class USelectionQueryManager;

	UPROPERTY()
	FName QueryName;

public:
	virtual  void PostInitProperties() override;

	/** QueryName patching up */
	virtual void PostLoad() override;
#if WITH_EDITOR
	virtual void PostDuplicate(bool bDuplicateForPIE) override;
#endif // WITH_EDITOR

	FName GetQueryName() const { return QueryName; }
};
