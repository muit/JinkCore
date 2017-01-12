// Copyright 2015-2017 Piperift. All Rights Reserved.

#include "JinkCorePrivatePCH.h"
#include "Entity.h"

#include "SummonList.h"

USummonList::USummonList()
{
}

void USummonList::Construct(AEntity * _Summoner)
{
    Summoner = _Summoner;
}

bool USummonList::Summon(UClass* Class, FTransform Transform) {
    if (IsAssigned()) {
        AEntity* Summon = Summoner->Summon(Class, Transform);
        if (Summon) {
            Add(Summon);
            return true;
        }
    }
    return false;
}

void USummonList::Add(AEntity* Summon) {
    if (Summon != Summoner) {
        Summons.AddUnique(Summon);
    }
}

UWorld* USummonList::GetWorld() const
{
    if (GetOuter())
        return GetOuter()->GetWorld();

    return Summoner? Summoner->GetWorld() : nullptr;
}
