// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "SelectionQueryComponent.h"
#include "SelectionQueryTypes.h"
#include "GameplayTaskOwnerInterface.h"
#include "Tasks/AITask.h"
#include "SQNode.generated.h"

JINKCORE_API DECLARE_LOG_CATEGORY_EXTERN(LogSelectionQuery, Display, All);

class USelectionQuery;
class USQCompositeNode;

UCLASS(Abstract)
class JINKCORE_API USQNode : public UObject
{
	GENERATED_UCLASS_BODY()

	/** Versioning for updating deprecated properties */
	UPROPERTY()
	int32 VerNum;

	virtual void UpdateNodeVersion();

	virtual FText GetDescriptionTitle() const;
	virtual FText GetDescriptionDetails() const;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif //WITH_EDITOR
};
