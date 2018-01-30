// Copyright 2015-2018 Piperift. All Rights Reserved.

#include "JinkCorePrivatePCH.h"
#include "Entity.h"
#include "JinkGameMode.h"

AJinkGameMode::AJinkGameMode()
{
    // set default pawn class to our Blueprinted character
    DefaultPawnClass = AEntity::StaticClass();
}
