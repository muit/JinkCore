// Copyright 2015-2017 Piperift. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "LevelInstance.h"
#include "LevelInstanceComponent.h"
#include "LIConector.generated.h"

struct FLIAnchor;

UCLASS(BlueprintType, meta = (DisplayName = "Level Instance Conector"))
class JINKCORE_API ALIConector : public AActor
{
	GENERATED_BODY()

    UBillboardComponent* SpriteComponent;
    // Icon sprite
    UTexture2D* SpriteTexture;

public:
    // Sets default values for this actor's properties
	ALIConector();

    virtual void SetupConAttachment(FLIAnchor& A, FLIAnchor& B);

    UFUNCTION(BlueprintImplementableEvent, Category = "Anchor")
    void OnAttachment(FLIAnchor& A, FLIAnchor& B);


	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
