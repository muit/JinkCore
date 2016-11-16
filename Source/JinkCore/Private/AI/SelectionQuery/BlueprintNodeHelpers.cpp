// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "JinkCorePrivatePCH.h"
#include "BlueprintNodeHelpers.h"

JINKCORE_API DECLARE_LOG_CATEGORY_EXTERN(LogSelectionQuery, Display, All);


namespace BlueprintNodeHelpers
{
	uint16 GetPropertiesMemorySize(const TArray<UProperty*>& PropertyData)
	{
		int32 TotalMem = 0;
		for (int32 PropertyIndex = 0; PropertyIndex < PropertyData.Num(); PropertyIndex++)
		{
			TotalMem += PropertyData[PropertyIndex]->GetSize();
		}

		if (TotalMem > MAX_uint16)
		{
			TotalMem = 0;
		}

		return TotalMem;
	}

#define GET_STRUCT_NAME_CHECKED(StructName) \
	((void)sizeof(StructName), TEXT(#StructName))

	bool CanUsePropertyType(const UProperty* TestProperty)
	{
		if (TestProperty->IsA(UNumericProperty::StaticClass()) ||
			TestProperty->IsA(UBoolProperty::StaticClass()) ||
			TestProperty->IsA(UNameProperty::StaticClass()))
		{
			return true;
		}

		const UStructProperty* StructProp = Cast<const UStructProperty>(TestProperty);
		if (StructProp)
		{
			FString CPPType = StructProp->GetCPPType(NULL, CPPF_None);
			if (CPPType.Contains(GET_STRUCT_NAME_CHECKED(FVector)) ||
				CPPType.Contains(GET_STRUCT_NAME_CHECKED(FRotator)))
			{
				return true;
			}
		}

		return false;
	}

	void CollectPropertyData(const UObject* Ob, const UClass* StopAtClass, TArray<UProperty*>& PropertyData)
	{
		UE_LOG(LogSelectionQuery, Verbose, TEXT("Looking for runtime properties of class: %s"), *GetNameSafe(Ob->GetClass()));

		PropertyData.Reset();
		for (UProperty* TestProperty = Ob->GetClass()->PropertyLink; TestProperty; TestProperty = TestProperty->PropertyLinkNext)
		{
			// stop when reaching base class
			if (TestProperty->GetOuter() == StopAtClass)
			{
				break;
			}

			// skip properties without any setup data
			if (TestProperty->HasAnyPropertyFlags(CPF_Transient) ||
				TestProperty->HasAnyPropertyFlags(CPF_DisableEditOnInstance) == false)
			{
				continue;
			}

			// serialize only simple types
			if (CanUsePropertyType(TestProperty))
			{
				UE_LOG(LogSelectionQuery, Verbose, TEXT("> name: '%s'"), *GetNameSafe(TestProperty));
				PropertyData.Add(TestProperty);
			}
		}
	}

#undef GET_STRUCT_NAME_CHECKED

	FString CollectPropertyDescription(const UObject* Ob, const UClass* StopAtClass, const TArray<UProperty*>& PropertyData)
	{
		FString RetString;
		for (UProperty* TestProperty = Ob->GetClass()->PropertyLink; TestProperty; TestProperty = TestProperty->PropertyLinkNext)
		{
			// stop when reaching base class
			if (TestProperty->GetOuter() == StopAtClass)
			{
				break;
			}

			// skip properties without any setup data	
			if (TestProperty->HasAnyPropertyFlags(CPF_Transient) ||
				TestProperty->HasAnyPropertyFlags(CPF_DisableEditOnInstance) ||
				PropertyData.Contains(TestProperty))
			{
				continue;
			}

			if (TestProperty->IsA(UClassProperty::StaticClass()) ||
				TestProperty->IsA(UStructProperty::StaticClass()) ||
				CanUsePropertyType(TestProperty))
			{
				if (RetString.Len())
				{
					RetString.AppendChar(TEXT('\n'));
				}

				const uint8* PropData = TestProperty->ContainerPtrToValuePtr<uint8>(Ob);
				RetString += DescribeProperty(TestProperty, PropData);
			}
		}

		return RetString;
	}

	void DescribeRuntimeValues(const UObject* Ob, const TArray<UProperty*>& PropertyData, TArray<FString>& RuntimeValues)
	{
		for (int32 PropertyIndex = 0; PropertyIndex < PropertyData.Num(); PropertyIndex++)
		{
			UProperty* TestProperty = PropertyData[PropertyIndex];
			const uint8* PropAddr = TestProperty->ContainerPtrToValuePtr<uint8>(Ob);

			RuntimeValues.Add(DescribeProperty(TestProperty, PropAddr));
		}
	}

	bool FindNodeOwner(AActor* OwningActor, UBTNode* Node, int32& OwningInstanceIdx)
	{
		bool bFound = false;

		APawn* OwningPawn = Cast<APawn>(OwningActor);
		if (OwningPawn && OwningPawn->Controller)
		{
			bFound = FindNodeOwner(OwningPawn->Controller, Node, OwningInstanceIdx);
		}
		return bFound;
	}

	void AbortLatentActions(UActorComponent& OwnerOb, const UObject& Ob)
	{
		if (!OwnerOb.HasAnyFlags(RF_BeginDestroyed) && OwnerOb.GetOwner())
		{
			UWorld* MyWorld = OwnerOb.GetOwner()->GetWorld();
			if (MyWorld)
			{
				MyWorld->GetLatentActionManager().RemoveActionsForObject(&Ob);
				MyWorld->GetTimerManager().ClearAllTimersForObject(&Ob);
			}
		}
	}

	//----------------------------------------------------------------------//
	// DEPRECATED
	//----------------------------------------------------------------------//
	void AbortLatentActions(UActorComponent* OwnerOb, const UObject* Ob)
	{
		if (OwnerOb && Ob)
		{
			AbortLatentActions(OwnerOb, Ob);
		}
	}

	bool HasBlueprintFunction(FName FuncName, const UObject* Object, const UClass* StopAtClass)
	{
		return Object && StopAtClass && HasBlueprintFunction(FuncName, *Object, *StopAtClass);
	}
}

