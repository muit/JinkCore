// Copyright 2015-2017 Piperift, All Rights Reserved.
#pragma once
#include "GameFramework/GameMode.h"
#include "Engine/StreamableManager.h"
#include "JinkGameMode.generated.h"

UCLASS()
class JINKCORE_API AJinkGameMode : public AGameMode
{
    GENERATED_BODY()

public:
    AJinkGameMode();

    FStreamableManager AssetLoader;
};



