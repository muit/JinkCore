// Copyright 2015-2017 Piperift. All Rights Reserved.

#include "JinkEditorPrivatePCH.h"
#if ENGINE_MINOR_VERSION <= 14 //If engine is 4.14 or older
#include "Editor/DetailCustomizations/Private/DetailCustomizationsPrivatePCH.h"
#endif

#include "Faction.h"
#include "JinkCore/Private/JinkCorePrivatePCH.h"
#include "JCGeneralSettings.h"

#include "FactionCustomization.h"

#define LOCTEXT_NAMESPACE "FFactionCustomization"

bool FFactionCustomization::CanCustomizeHeader(TSharedRef<class IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
    StructHandle = StructPropertyHandle;
    TypeHandle = StructPropertyHandle->GetChildHandle("Name");

    if (TypeHandle->IsValidHandle()) {
        if (FJinkCoreModule* JinkCoreModule = FJinkCoreModule::GetInstance())
        {
            //Bind On Settings Changed event
            JinkCoreModule->OnModifiedGeneralSettings().BindRaw(this, &FFactionCustomization::UpdateItems);
        }
        return true;
    }
    return false;
}

TArray<FString> FFactionCustomization::GetEnumItems() {
    TArray<FString> Values = GetDefault<UJCGeneralSettings>()->Factions;
    // Make sure None is at the start
    Values.Remove(FACTION_None);
    Values.Insert(FACTION_None, 0);

    return Values;
}

void FFactionCustomization::OnItemSelected(FString Value) {
    TypeHandle->SetValue(Value);
}

/** Display the current column selection */
FText FFactionCustomization::GetSelectedItem() const
{
    FString ContainedValue;
    const FPropertyAccess::Result RowResult = TypeHandle->GetValue(ContainedValue);

    if (RowResult != FPropertyAccess::MultipleValues)
    {
        if (!ContainedValue.IsEmpty()) {
            return FText::FromString(*ContainedValue);
        }
        return FText::FromString(FACTION_None);
    }
    return LOCTEXT("MultipleValues", "Multiple Values");
}

#undef LOCTEXT_NAMESPACE
