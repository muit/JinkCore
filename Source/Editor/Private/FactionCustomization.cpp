// Copyright 2015-2016 Piperift. All Rights Reserved.

#include "JinkEditorPrivatePCH.h"
#include "Editor/DetailCustomizations/Private/DetailCustomizationsPrivatePCH.h"

#include "Faction.h"
#include "JinkCore/Private/JinkCorePrivatePCH.h"
#include "JinkCoreSettings.h"

#include "FactionCustomization.h"

#define LOCTEXT_NAMESPACE "FFactionCustomization"

void FFactionCustomization::CustomizeHeader(TSharedRef<class IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils) 
{
	StructHandle = StructPropertyHandle;
	NameHandle = StructPropertyHandle->GetChildHandle("Name");

	if (NameHandle->IsValidHandle()) {
		if (FJinkCoreModule* JinkCoreModule = FModuleManager::GetModulePtr<FJinkCoreModule>("JinkCore"))
		{
			//Bind On Settings Changed event
			JinkCoreModule->OnModifiedSettings().BindRaw(this, &FFactionCustomization::OnSettingsChanged);
		}

		UpdateFactionNames();
		
		HeaderRow.NameContent()
		[
			StructPropertyHandle->CreatePropertyNameWidget()
		]
		.ValueContent()
		.MaxDesiredWidth(0.0f)
		.MinDesiredWidth(125.0f)
		[
			SAssignNew(ComboBox, SComboBox<TSharedPtr<FString>>)
			.OptionsSource(&FactionNames)
			.OnGenerateWidget(this, &FFactionCustomization::HandleFactionNameComboBoxGenerateWidget)
			.OnSelectionChanged(this, &FFactionCustomization::OnSelectionChanged)
			//.InitiallySelectedItem(GetVariableFactionValue())
			[
				SNew(STextBlock)
				.Text(this, &FFactionCustomization::GetFactionNameComboBoxContentText)
			]
		];
	}

}


void FFactionCustomization::CustomizeChildren(TSharedRef<class IPropertyHandle> StructPropertyHandle, class IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	//Do Nothing
}



/** Return the representation of the the column names to display */
TSharedRef<SWidget> FFactionCustomization::HandleFactionNameComboBoxGenerateWidget(TSharedPtr<FString> Item)
{
	return SNew(STextBlock)
		.Text(FText::FromString(*Item));
}

/** Display the current column selection */
FText FFactionCustomization::GetFactionNameComboBoxContentText() const
{
	FString ContainedValue;
	const FPropertyAccess::Result RowResult = NameHandle->GetValue(ContainedValue);

	if (RowResult != FPropertyAccess::MultipleValues)
	{
		if (!ContainedValue.IsEmpty()) {
			return FText::FromString(*ContainedValue);
		}
		return FText::FromString(FACTION_None);
	}
	return LOCTEXT("MultipleValues", "Multiple Values");
}

/** Update the root data on a change of selection */
void FFactionCustomization::OnSelectionChanged(TSharedPtr<FString> SelectedItem, ESelectInfo::Type SelectInfo)
{
	if (SelectedItem.IsValid()) {
		FString NewValue = **SelectedItem;

		UpdateFactionNames();

		NameHandle->SetValue(NewValue);
	}
}

void FFactionCustomization::OnSettingsChanged()
{
	UpdateFactionNames();
}

/** Display the current column selection */
void FFactionCustomization::UpdateFactionNames()
{
	Names = GetDefault<UJinkCoreSettings>()->Factions;
	Names.Remove(FACTION_None);
	Names.Insert(FACTION_None, 0);

	FactionNames.Empty();
	
	//Convert FString to Shared Ptrs and Populate the array
	for (auto It = Names.CreateConstIterator(); It; ++It)
	{
		if (!(*It).IsEmpty())
		{
			TSharedPtr<FString> Name = MakeShareable(new FString(*It));
			FactionNames.Add(Name);
		}
	}

	if (ComboBox.IsValid()) {
		ComboBox->RefreshOptions();
	}
}


#undef LOCTEXT_NAMESPACE