// Copyright 2015-2017 Piperift. All Rights Reserved.

#pragma once

#include "Factories/Factory.h"
#include "LevelInstanceFactory.generated.h"

UCLASS()
class ULevelInstanceFactory : public UFactory
{
	GENERATED_UCLASS_BODY()



	// UFactory interface
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	virtual uint32 GetMenuCategories() const override;
	// End of UFactory interface
};
