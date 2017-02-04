// Copyright 2015-2017 Piperift. All Rights Reserved.

#pragma once

#include "SummonList.generated.h"

class AEntity;

/**
 * 
 */
UCLASS(BlueprintType)
class JINKCORE_API USummonList : public UObject
{
    GENERATED_BODY()

public:
    USummonList();

    UFUNCTION(BlueprintCallable, Category = SummonList)
    void Construct(AEntity* _Summoner);

private:
    UPROPERTY()
    AEntity* Summoner;
protected:
    UPROPERTY()
    TArray<AEntity*> Summons;

public:
    UFUNCTION(BlueprintCallable, Category = SummonList)
    bool Summon(UClass* Class, FTransform Transform);
    UFUNCTION(BlueprintCallable, Category = SummonList)
    void Add(AEntity* Summon);

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = SummonList)
    FORCEINLINE bool IsAssigned() const {
        return Summoner != nullptr;
    }

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = SummonList)
    FORCEINLINE AEntity* GetSummoner() {
        return Summoner;
    }

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = SummonList)
    FORCEINLINE TArray<AEntity*> ToArray() {
        return Summons;
    }

    virtual UWorld* GetWorld() const override;
};
