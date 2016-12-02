// Copyright 2015-2017 Piperift. All Rights Reserved.

#pragma once

#include "Factories/Factory.h"
#include "ComplexItemFactory.generated.h"

/**
 * 
 */
UCLASS()
class UComplexItemFactory : public UFactory
{
    GENERATED_UCLASS_BODY()

    // UFactory interface
    virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;

    /** When shown in menus, this is the category containing this factory. Return type is a BitFlag mask using EAssetTypeCategories. */
    virtual uint32 GetMenuCategories() const override;
    // End of UFactory interface
};
