// Copyright 2015-2017 Piperift. All Rights Reserved.

#pragma once

#include "LIAnchorTypeInfo.generated.h"


USTRUCT(BlueprintType)
struct FLIAnchorActor
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TAssetPtr<AActor> Actor;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FTransform Offset;

	FLIAnchorActor() : Offset(FTransform::Identity)
	{}
};

USTRUCT(BlueprintType)
struct FLIAnchorMesh
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TAssetPtr<UStaticMesh> Mesh;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FTransform Offset;

	FLIAnchorMesh() : Offset(FTransform::Identity)
	{}
};

/**
 * 
 */
USTRUCT(BlueprintType, meta = (DisplayName = "Level Instance Anchor Type Info"))
struct FLIAnchorTypeInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Anchor Type")
	FColor Color;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Anchor Type")
	TArray<FLIAnchorActor> AnchorActors;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Anchor Type")
	TArray<FLIAnchorMesh> AnchorMeshes;
	
	FLIAnchorTypeInfo() : Color(FColor::Orange)
	{}
};
