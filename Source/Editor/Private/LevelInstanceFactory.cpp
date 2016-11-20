// Copyright 2015-2017 Piperift. All Rights Reserved.

#include "JinkEditorPrivatePCH.h"
#include "JinkCore.h"
#include "LevelInstance.h"

#include "LevelInstanceFactory.h"

ULevelInstanceFactory::ULevelInstanceFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SupportedClass = ULevelInstance::StaticClass();
	Formats.Add(TEXT("t3d;Unreal World"));

	bText = true;
	bCreateNew = true;
	bEditorImport = true;
	bEditAfterNew = true;
}

UObject* ULevelInstanceFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<ULevelInstance>(InParent, ULevelInstance::StaticClass(), Name, Flags);
}

uint32 ULevelInstanceFactory::GetMenuCategories() const
{
	if (FJinkCoreModule* JC = FJinkCoreModule::GetInstance()) {
		return JC->GetJCAssetCategoryBit();
	}
	// If JinkCore module is not found use Miscellaneous 
	return EAssetTypeCategories::Misc;
}
