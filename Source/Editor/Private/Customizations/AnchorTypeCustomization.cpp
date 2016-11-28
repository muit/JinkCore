// Copyright 2015-2016 Piperift. All Rights Reserved.

#include "JinkEditorPrivatePCH.h"
#include "Editor/DetailCustomizations/Private/DetailCustomizationsPrivatePCH.h"

#include "LIAnchorType.h"
#include "JinkCore/Private/JinkCorePrivatePCH.h"
#include "JCGenerationSettings.h"

#include "AnchorTypeCustomization.h"

#define LOCTEXT_NAMESPACE "FAnchorTypeCustomization"

/** Called at customization's setup */
bool FAnchorTypeCustomization::CanCustomizeHeader(TSharedRef<class IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	StructHandle = StructPropertyHandle;
	TypeHandle = StructPropertyHandle->GetChildHandle("Name");
	IdHandle = StructPropertyHandle->GetChildHandle("Id");

	if (TypeHandle->IsValidHandle() && 
		IdHandle->IsValidHandle())
	{
		if (FJinkCoreModule* JinkCoreModule = FJinkCoreModule::GetInstance())
		{
			//Bind On Settings Changed event
			JinkCoreModule->OnModifiedGenerationSettings().BindRaw(this, &FAnchorTypeCustomization::UpdateItems);
		}
		return true;
	}
	return false;
}

TArray<FString> FAnchorTypeCustomization::GetEnumItems() {
	TArray<FString> Values;
	GetDefault<UJCGenerationSettings>()->AnchorTypes.GetKeys(Values);

    // Make sure None is at the start
    Values.Remove(ANCHOR_None);
    Values.Insert(ANCHOR_None, 0);

	return Values;
}

void FAnchorTypeCustomization::OnItemSelected(FString Value){

	TArray<FString> Values;
	GetDefault<UJCGenerationSettings>()->AnchorTypes.GetKeys(Values);

	TypeHandle->SetValue(Value);
	if (Values.Contains(Value)) {
		IdHandle->SetValue(Values.IndexOfByKey(Value));
	}
	else {
		IdHandle->SetValue(0);
	}
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