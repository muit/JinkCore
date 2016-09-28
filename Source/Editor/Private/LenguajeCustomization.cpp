// Copyright 2015-2016 Piperift. All Rights Reserved.

#include "JinkCorePrivatePCH.h"
#include "LenguajeCustomization.h"

#define LOCTEXT_NAMESPACE "LenguajeCustomization"

TSharedRef<IPropertyTypeCustomization> FLenguajeCustomization::MakeInstance()
{
	return MakeShareable(new FLenguajeCustomization());
}

void FLenguajeCustomization::CustomizeChildren(TSharedRef<class IPropertyHandle> StructPropertyHandle, class IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	//Create further customization here
}

#undef LOCTEXT_NAMESPACE