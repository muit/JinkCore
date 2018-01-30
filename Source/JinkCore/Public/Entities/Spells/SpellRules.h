// Copyright 2015-2018 Piperift. All Rights Reserved.

#pragma once

#include "Spell.h"
#include "SpellRules.generated.h"

/**
 * Unique casting rules for each spell. E.g Charges
 */
USTRUCT(Blueprintable)
struct JINKCORE_API FSpellRules
{
    GENERATED_USTRUCT_BODY()
public:

    //UFUNCTION(BlueprintNativeEvent)
    //bool CanCastSpell(TSubclassOf<ASpell> SpellClass);


    //TODO: Override GetWorld()
};
