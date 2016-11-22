// Copyright 2015-2017 Piperift. All Rights Reserved.

#pragma once

#include "LevelInstance.generated.h"


USTRUCT(BlueprintType, meta = (DisplayName = "Level Instance Anchor"))
struct FLIAnchor
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "")
	FName Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transform")
	FTransform Transform;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Transform")
	FGuid GUID;

	FLIAnchor() : Transform(FTransform::Identity), Name("None"), GUID(FGuid::NewGuid())
	{
	
	}
};

/**
 * 
 */
UCLASS(Blueprintable)
class JINKCORE_API ULevelInstance : public UObject
{
	GENERATED_BODY()

public:

	ULevelInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "Level Instance")
	void SetupBounds();


	//~ Begin Level Instance Interface
private:
	UPROPERTY()
	ULevelStreamingKismet* StreamingLevel;

public:
	static int32 InstanceIdCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Instance")
	TAssetPtr<UWorld> InstancedLevel;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Level Instance")
    FBox Bounds;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level Instance|Settings")
	bool bShouldBeLoaded;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level Instance|Settings")
	bool bShouldBeVisible;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level Instance|Settings")
	bool bShouldBlockOnLoad;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level Instance|Settings")
	bool bInitiallyLoaded;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level Instance|Settings")
	bool bInitiallyVisible;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Level Instance")
	TArray<FLIAnchor> Anchors;
	//~ End Level Instance Interface
};
