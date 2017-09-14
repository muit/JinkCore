// Copyright 2015-2017 Piperift, All Rights Reserved.
#pragma once
#include "GameFramework/GameMode.h"
#include "Engine/StreamableManager.h"
#include "JinkGameMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGamePaused);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameUnpaused);

UCLASS()
class JINKCORE_API AJinkGameMode : public AGameMode
{
    GENERATED_BODY()

public:
    AJinkGameMode();

    UFUNCTION(BlueprintCallable)
    FORCEINLINE bool Pause() {
        const bool Paused = UGameplayStatics::SetGamePaused(this, true);

        if(Paused)
            OnGamePaused.Broadcast();
        return Paused;
    }

    UFUNCTION(BlueprintCallable)
    FORCEINLINE bool Unpause() {
        const bool Resumed = UGameplayStatics::SetGamePaused(this, false);

        if (Resumed)
            OnGameUnpaused.Broadcast();
        return Resumed;
    }

    UPROPERTY(BlueprintAssignable)
    FOnGamePaused OnGamePaused;
    UPROPERTY(BlueprintAssignable)
    FOnGameUnpaused OnGameUnpaused;
};



