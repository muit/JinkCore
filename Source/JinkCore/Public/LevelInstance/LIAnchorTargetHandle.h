// Copyright 2015-2017 Piperift. All Rights Reserved.

#pragma once

#include "Engine/TargetPoint.h"
#include "Components/ArrowComponent.h"
#include "LIAnchorTargetHandle.generated.h"

struct FLIAnchorType;
struct FLIAnchor;
class ALevelInstanceBounds;

/**
 * 
 */
UCLASS()
class JINKCORE_API ALIAnchorTargetHandle : public ATargetPoint
{
    GENERATED_UCLASS_BODY()

public:
    UPROPERTY()
    ALevelInstanceBounds* BoundsOwner;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anchor")
    FName Name;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anchor")
    FLIAnchorType Type;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anchor")
    FGuid GUID;


    FLIAnchor GetAsAnchor();

#if WITH_EDITOR
    virtual void PostEditUndo() override;
    virtual void PostEditMove(bool bFinished) override;
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

    void UpdateLIBounds();
#endif //WITH_EDITOR
};
