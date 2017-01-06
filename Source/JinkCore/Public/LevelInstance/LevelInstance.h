// Copyright 2015-2017 Piperift. All Rights Reserved.

#pragma once

#include "LIAnchorType.h"

#include "LevelInstance.generated.h"


USTRUCT(BlueprintType, meta = (DisplayName = "Level Instance Anchor"))
struct FLIAnchor
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "")
    FName Name;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "")
    FLIAnchorType Type;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "")
    FTransform Transform;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "")
    FGuid GUID;

    FLIAnchor() : Name("Conector"), Transform(FTransform::Identity), GUID(FGuid::NewGuid())
    {}

    void CopyFrom(const FLIAnchor& Other) {
        GUID = Other.GUID;
        Name = Other.Name;
        Type = Other.Type;
        Transform = Other.Transform;
    }

    FORCEINLINE bool operator==(const FLIAnchor &Other) const
    {
        return GUID == Other.GUID;
    }

    FORCEINLINE bool operator==(const FGuid &OtherGUID) const
    {
        return GUID == OtherGUID;
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
