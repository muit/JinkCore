// Copyright 2015-2016 Piperift. All Rights Reserved.
#pragma once

#include "IPropertyTypeCustomization.h"

class FFactionCustomization : public IPropertyTypeCustomization
{
public:
	/**
	* Creates a new instance.
	*
	* @return A new struct customization for Factions.
	*/
	static TSharedRef<IPropertyTypeCustomization> MakeInstance() 
	{
		return MakeShareable(new FFactionCustomization);
	}

	/** IPropertyTypeCustomization interface */
	virtual void CustomizeHeader(TSharedRef<class IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<class IPropertyHandle> StructPropertyHandle, class IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;

private:
	/** Return the representation of the the column names to display */
	TSharedRef<SWidget> HandleFactionNameComboBoxGenerateWidget(TSharedPtr<FString> Item);

	/** Display the current column selection */
	FText GetFactionNameComboBoxContentText() const;

	FString FFactionCustomization::GetFactionNameComboBoxContentString() const;

	/** Update the root data on a change of selection */
	void OnSelectionChanged(TSharedPtr<FString> SelectedItem, ESelectInfo::Type SelectInfo);


	void UpdateFactionNames();

	/** The row combobox object */
	TSharedPtr<SComboBox<TSharedPtr<FString>>> ComboBox;
	/** Handle to the struct properties being customized */
	TSharedPtr<IPropertyHandle> StructHandle;
	TSharedPtr<IPropertyHandle> NameHandle;
	/** A cached copy of strings to populate the column combo box */
	TArray<TSharedPtr<FString>> FactionNames;
};

