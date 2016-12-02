// Copyright 2015-2017 Piperift. All Rights Reserved.

#include "JinkCorePrivatePCH.h"
#include "Item.h"

#define LOCTEXT_NAMESPACE "Item"

UItem::UItem(const FObjectInitializer & ObjectInitializer)
    : Super(ObjectInitializer)
{
    DisplayName = "Item";
    Description = LOCTEXT("Description", "");

    //Properties
    DamageIncrement = 0;
    LiveIncrement = 0;
    MovementSpeedCof = 1;
    FireRateCof = 1;
    BulletSpeedCof = 1;
}

void UItem::DoTest_Implementation() {
}

//////////////////////////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE