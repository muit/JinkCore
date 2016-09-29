// Copyright 2015-2016 Piperift. All Rights Reserved.

#include "JinkEditorPrivatePCH.h"

#include "FactionCustomization.h"

DEFINE_LOG_CATEGORY(JinkEditor)
 
#define LOCTEXT_NAMESPACE "JinkEditor"
 
void FJinkEditorModule::StartupModule()
{
	UE_LOG(JinkEditor, Warning, TEXT("JinkEditor: Log Started"));

	RegisterPropertyTypeCustomizations();
}
 
void FJinkEditorModule::ShutdownModule()
{
	UE_LOG(JinkEditor, Warning, TEXT("JinkEditor: Log Ended"));
}

void FJinkEditorModule::RegisterPropertyTypeCustomizations()
{
	RegisterCustomPropertyTypeLayout("Faction", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FFactionCustomization::MakeInstance));
}


void FJinkEditorModule::RegisterCustomPropertyTypeLayout(FName PropertyTypeName, FOnGetPropertyTypeCustomizationInstance PropertyTypeLayoutDelegate)
{
	check(PropertyTypeName != NAME_None);

	static FName PropertyEditor("PropertyEditor");
	FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>(PropertyEditor);
	PropertyModule.RegisterCustomPropertyTypeLayout(PropertyTypeName, PropertyTypeLayoutDelegate);
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_GAME_MODULE(FJinkEditorModule, JinkEditor);