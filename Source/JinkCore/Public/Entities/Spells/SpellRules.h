// Copyright 2015-2017 Piperift. All Rights Reserved.

#pragma once

#include "Spell.h"
#include "SpellRules.generated.h"

/**
 * Unique casting rules for each spell. E.g Charges
 */
UCLASS(Blueprintable)
class JINKCORE_API USpellRules : public UObject
{
    GENERATED_UCLASS_BODY()
public:

    //UFUNCTION(BlueprintNativeEvent)
    //bool CanCastSpell(TSubclassOf<ASpell> SpellClass);


    //TODO: Override GetWorld()
};
