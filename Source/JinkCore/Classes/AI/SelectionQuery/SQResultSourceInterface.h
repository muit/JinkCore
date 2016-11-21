// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "SQResultSourceInterface.generated.h"

UINTERFACE(MinimalAPI, meta=(CannotImplementInterfaceInBlueprint))
class USQResultSourceInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class ISQResultSourceInterface
{
	GENERATED_IINTERFACE_BODY()

	virtual const struct FSQResult* GetQueryResult() const { return NULL; }
	virtual const struct FSelectionQueryInstance* GetQueryInstance() const { return NULL; }
};
