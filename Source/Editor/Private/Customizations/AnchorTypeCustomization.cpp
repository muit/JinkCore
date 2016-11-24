// Copyright 2015-2016 Piperift. All Rights Reserved.

#include "JinkEditorPrivatePCH.h"

#include "LIAnchorType.h"
#include "JinkCore/Private/JinkCorePrivatePCH.h"
#include "JinkCoreSettings.h"

#include "AnchorTypeCustomization.h"

#define LOCTEXT_NAMESPACE "FAnchorTypeCustomization"

/** Called at customization's setup */
bool FAnchorTypeCustomization::CanCustomizeHeader(TSharedRef<class IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils){
	StructHandle = StructPropertyHandle;
	TypeHandle = StructPropertyHandle->GetChildHandle("Name");

	if (TypeHandle->IsValidHandle()) {
		if (FJinkCoreModule* JinkCoreModule = FJinkCoreModule::GetInstance())
		{
			//Bind On Settings Changed event
			JinkCoreModule->OnModifiedSettings().BindRaw(this, &FStringEnumCustomization::UpdateItems);
		}
		return true;
	}
	return false;
}

TArray<FString> FAnchorTypeCustomization::GetEnumItems() {
	TArray<FString> Values;
	Values.Add(FString("None"));
	return Values;
}

void FAnchorTypeCustomization::OnItemSelected(FString Value){
	TypeHandle->SetValue(Value);
}

/** Display the current column selection */
FText FAnchorTypeCustomization::GetSelectedItem() const
{
	FString ContainedValue;
	const FPropertyAccess::Result RowResult = TypeHandle->GetValue(ContainedValue);

	if (RowResult != FPropertyAccess::MultipleValues)
	{
		if (!ContainedValue.IsEmpty()) {
			return FText::FromString(*ContainedValue);
		}
		return FText::FromString(ANCHOR_None);
	}
	return LOCTEXT("MultipleValues", "Multiple Values");
}

#undef LOCTEXT_NAMESPACE