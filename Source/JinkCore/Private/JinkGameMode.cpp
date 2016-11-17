// Copyright 2015-2016 Piperift. All Rights Reserved.

#include "JinkCorePrivatePCH.h"
#include "Entity.h"
#include "AI/SelectionQuery/SelectionQueryManager.h"
#include "JinkGameMode.h"

AJinkGameMode::AJinkGameMode()
{
	// set default pawn class to our Blueprinted character
	DefaultPawnClass = AEntity::StaticClass();

    //Create Selection Query Manager
    UWorld* WorldOuter = Cast<UWorld>(GetOuter());
    UObject* ManagersOuter = WorldOuter != NULL ? (UObject*)WorldOuter : (UObject*)this;
    SelectionQueryManager = NewObject<USelectionQueryManager>(ManagersOuter);


    FWorldDelegates::OnWorldCleanup.AddDynamic(this, &AJinkGameMode::CleanupWorld);
}

void AJinkGameMode::CleanupWorld(UWorld * World, bool bSessionEnded, bool bCleanupResources)
{
    if (bCleanupResources)
    {
        if (SelectionQueryManager)
        {
            SelectionQueryManager->OnWorldCleanup();
            SelectionQueryManager = nullptr;
        }
    }
}
