// Copyright 2015-2016 Piperift. All Rights Reserved.

#include "JinkCorePrivatePCH.h"

#include "LIAnchorTypeInfo.h"
#include "JCGenerationSettings.h"

#include "LIAnchorType.h"

bool FLIAnchorType::GetAnchorInfo(FLIAnchorTypeInfo& Info) {
    const UJCGenerationSettings* Settings = GetDefault<UJCGenerationSettings>();

    if (Settings->AnchorTypes.Contains(Name)) {
        Info = Settings->AnchorTypes[Name];
        //If the type is not found, return default type info.
        return true;
    }

    Info = FLIAnchorTypeInfo();
    return false;
}