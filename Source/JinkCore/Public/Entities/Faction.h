// Copyright 2015-2016 Piperift. All Rights Reserved.

#pragma once

#include "Object.h"
#include "Faction.generated.h"

#define FACTION_None FString("None")

/**
 * 
 */
USTRUCT(BlueprintType)
struct JINKCORE_API FFaction
{
    GENERATED_USTRUCT_BODY()

    FFaction() : Name(FACTION_None) 
    {}

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Faction")
    FString Name;


    FORCEINLINE bool operator==(FFaction& Other) const {
        return Other.Name.Equals(Name);
    }

    FORCEINLINE bool operator!=(FFaction& Other) const {
        return !Other.Name.Equals(Name);
    }

    bool IsHostileTo(FFaction& Other) const {
        //If are different factions or are None it's hostile
        return *this != Other || Other.Name == FACTION_None;
    }
};
