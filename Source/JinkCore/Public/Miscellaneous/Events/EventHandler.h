// Copyright 2015-2016 Piperift. All Rights Reserved.

#pragma once

#include "Object.h"
#include "EventHandler.generated.h"

DECLARE_DELEGATE_OneParam(FEventDelegate, int);

/**
 * 
 */
USTRUCT()
struct JINKCORE_API FEventHandler
{
    GENERATED_USTRUCT_BODY()

public:
    FEventHandler() {
        bValid = false;
        bActivated = false;
        Id = 0;
    }

    FEventHandler(UObject* _Outer, int _Id);

protected:
    UPROPERTY()
    bool bValid;
    UPROPERTY()
    bool bPaused;

    FTimer* Timer;

    UPROPERTY(BlueprintReadOnly, Category = "Timer")
    float Length;

public:

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timer")
    int Id;
    UPROPERTY(BlueprintReadOnly, Category = "Timer")
    bool bActivated;

    UPROPERTY()
    UObject* Outer;

    FEventDelegate EventDelegate;


    template< class UserClass >
    void Bind(UserClass* Context, typename FEventDelegate::TUObjectMethodDelegate< UserClass >::FMethodPtr InEventMethod);

    // Start the event timer
    void Start(float Length);

private:
    void StartInternal(int Length);

public:
    // Pause the event timer
    void Pause();

    // Resume the event timer
    void Resume();

    //Reset the event and start it.
    void Restart(float Length);
    //Reset the event and start it.
    void Reset();

    //Called when the event is done.
    void OnExecute();

    // HELPERS
    bool const IsValid() { return bValid; }
    bool const IsRunning() { return bActivated; }
    bool const IsPaused() { return IsRunning() && bPaused; }
    float const GetLength() { return Length; }

    void Tick(float DeltaTime);

    virtual class UWorld* GetWorld() const{
        return  Outer ? Outer->GetWorld() : nullptr;;
    }
};
