// Copyright 2015-2016 Piperift. All Rights Reserved.

#pragma once

#include "Object.h"
#include "RegisterAtom.h"
#include "MissionRow.generated.h"

USTRUCT(Blueprintable)
struct JINKCORE_API FMissionRow : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	FMissionRow()
		: Title("")
		, Description(FText::FromString(""))
	{}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Title;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Description;

	/*
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FString> MissionsRequired;
	*/

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TAssetPtr<URegisterAtom>> ProgressAtoms;
};