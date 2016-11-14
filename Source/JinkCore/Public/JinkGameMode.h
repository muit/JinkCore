// Copyright 2015-2016 Piperift, All Rights Reserved.
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


    /** Environment query manager used by game */
    UPROPERTY(Transient)
    USelectionQueryManager* SelectionQueryManager;

    /** Environment Query manager getter */
    FORCEINLINE USelectionQueryManager* GetSelectionQueryManager() { return SelectionQueryManager; }
    /** Environment Query manager const getter */
    FORCEINLINE const USelectionQueryManager* GetSelectionQueryManager() const { return SelectionQueryManager; }
};



