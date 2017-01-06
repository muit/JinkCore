// Copyright 2015-2017 Piperift. All Rights Reserved.

#pragma once

#include "Factories/Factory.h"
#include "SelectionQueryFactory.generated.h"

UCLASS()
class USelectionQueryFactory : public UFactory
{
    GENERATED_UCLASS_BODY()


    // Initial level to create the sprite from (Can be nullptr)
    class TAssetPtr<UWorld> InitialLevel;

    // UFactory interface
    virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
    // End of UFactory interface
};
