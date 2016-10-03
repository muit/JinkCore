// Copyright 2015-2016 Piperift. All Rights Reserved.

#pragma once

#include "Engine/DataAsset.h"
#include "RegisterAtom.generated.h"

/**
 * 
 */
UCLASS()
class JINKCORE_API URegisterAtom : public UDataAsset
{
    GENERATED_BODY()
    
    URegisterAtom()
		: Name("")
		, Amount(1)
	{}

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mission")
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mission")
	uint8 Amount;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mission")
	//TAssetPtr<Requ> AchievementIcon;
};