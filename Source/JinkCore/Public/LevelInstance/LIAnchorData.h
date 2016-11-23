// Copyright 2015-2017 Piperift. All Rights Reserved.

#pragma once

#include "UObject/NoExportTypes.h"
#include "LIAnchorData.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class JINKCORE_API ULIAnchorData : public UObject
{
	GENERATED_BODY()

public:
    ULIAnchorData(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "")
    FName Name;
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "")
    FTransform Transform;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "")
    FGuid GUID;

    FORCEINLINE bool operator==(const ULIAnchorData &Other) const
    {
        return GUID == Other.GUID;
    }

    FORCEINLINE bool operator==(const FGuid &OtherGUID) const
    {
        return GUID == OtherGUID;
    }
	
};
