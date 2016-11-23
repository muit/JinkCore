// Copyright 2015-2017 Piperift. All Rights Reserved.

#include "JinkCorePrivatePCH.h"
#include "LIAnchorData.h"


ULIAnchorData::ULIAnchorData(const FObjectInitializer & ObjectInitializer)
    : Super(ObjectInitializer)
{
    Name = "None";
    Transform = FTransform::Identity;
    GUID = FGuid::NewGuid();
}

