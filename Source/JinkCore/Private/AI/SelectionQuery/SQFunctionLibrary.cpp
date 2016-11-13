// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "JinkCorePrivatePCH.h"
#include "SelectionQuery/SQFunctionLibrary.h"
#include "SelectionQuery/BlackboardComponent.h"
#include "SelectionQuery/Blackboard/BlackboardKeyAllTypes.h"
#include "SelectionQuery/SQFunctionLibrary.h"

//----------------------------------------------------------------------//
// 
//----------------------------------------------------------------------//
namespace FSQNodeBPImplementationHelper
{
	int32 CheckEventImplementationVersion(FName GenericEventName, FName AIEventName, const UObject& Object, const UClass& StopAtClass)
	{
		const bool bGeneric = BlueprintNodeHelpers::HasBlueprintFunction(GenericEventName, Object, StopAtClass);
		const bool bAI = BlueprintNodeHelpers::HasBlueprintFunction(AIEventName, Object, StopAtClass);

		return (bGeneric ? Generic : NoImplementation) | (bAI ? AISpecific : NoImplementation);
	}

	int32 CheckEventImplementationVersion(FName GenericEventName, FName AIEventName, const UObject* Ob, const UClass* StopAtClass)
	{
		return (Ob && StopAtClass) ? CheckEventImplementationVersion(GenericEventName, AIEventName, *Ob, *StopAtClass) : NoImplementation;
	}
}

//----------------------------------------------------------------------//
// 
//----------------------------------------------------------------------//
namespace
{
	FORCEINLINE UBlackboardComponent* GetBlackboard(USQNode& SQNode)
	{
		check(SQNode.GetOuter());
		check(SQNode.GetClass()->HasAnyClassFlags(CLASS_CompiledFromBlueprint) || Cast<UDynamicClass>(SQNode.GetClass()) && "This function call is valid only for BP-implemented SQ nodes");
		check(Cast<USelectionQueryComponent>(SQNode.GetOuter()));

		return ((USelectionQueryComponent*)SQNode.GetOuter())->GetBlackboardComponent();
	}
}

USelectionQueryComponent* USQFunctionLibrary::GetOwnerComponent(USQNode* NodeOwner)
{
	check(NodeOwner);
	check(NodeOwner->GetClass()->HasAnyClassFlags(CLASS_CompiledFromBlueprint) && "This function call is valid only for BP-implemented SQ nodes");
	check(NodeOwner->GetOuter());

	USelectionQueryComponent* OwnerComp = Cast<USelectionQueryComponent>(NodeOwner->GetOuter());
	check(OwnerComp);

	return OwnerComp;
}

UBlackboardComponent* USQFunctionLibrary::GetOwnersBlackboard(USQNode* NodeOwner)
{
	check(NodeOwner != NULL);
	return GetBlackboard(*NodeOwner);
}

UObject* USQFunctionLibrary::GetBlackboardValueAsObject(USQNode* NodeOwner, const FBlackboardKeySelector& Key)
{
	check(NodeOwner != NULL);
	const UBlackboardComponent* BlackboardComp = GetBlackboard(*NodeOwner);
	return BlackboardComp ? BlackboardComp->GetValueAsObject(Key.SelectedKeyName) : NULL;
}

AActor* USQFunctionLibrary::GetBlackboardValueAsActor(USQNode* NodeOwner, const FBlackboardKeySelector& Key)
{
	check(NodeOwner != NULL);
	const UBlackboardComponent* BlackboardComp = GetBlackboard(*NodeOwner);
	return BlackboardComp ? Cast<AActor>(BlackboardComp->GetValueAsObject(Key.SelectedKeyName)) : NULL;
}

UClass* USQFunctionLibrary::GetBlackboardValueAsClass(USQNode* NodeOwner, const FBlackboardKeySelector& Key)
{
	check(NodeOwner != NULL);
	const UBlackboardComponent* BlackboardComp = GetBlackboard(*NodeOwner);
	return BlackboardComp ? BlackboardComp->GetValueAsClass(Key.SelectedKeyName) : NULL;
}

uint8 USQFunctionLibrary::GetBlackboardValueAsEnum(USQNode* NodeOwner, const FBlackboardKeySelector& Key)
{
	check(NodeOwner != NULL);
	const UBlackboardComponent* BlackboardComp = GetBlackboard(*NodeOwner);
	return BlackboardComp ? BlackboardComp->GetValueAsEnum(Key.SelectedKeyName) : 0;
}

int32 USQFunctionLibrary::GetBlackboardValueAsInt(USQNode* NodeOwner, const FBlackboardKeySelector& Key)
{
	check(NodeOwner != NULL);
	const UBlackboardComponent* BlackboardComp = GetBlackboard(*NodeOwner);
	return BlackboardComp ? BlackboardComp->GetValueAsInt(Key.SelectedKeyName) : 0;
}

float USQFunctionLibrary::GetBlackboardValueAsFloat(USQNode* NodeOwner, const FBlackboardKeySelector& Key)
{
	check(NodeOwner != NULL);
	const UBlackboardComponent* BlackboardComp = GetBlackboard(*NodeOwner);
	return BlackboardComp ? BlackboardComp->GetValueAsFloat(Key.SelectedKeyName) : 0.0f;
}

bool USQFunctionLibrary::GetBlackboardValueAsBool(USQNode* NodeOwner, const FBlackboardKeySelector& Key)
{
	check(NodeOwner != NULL);
	const UBlackboardComponent* BlackboardComp = GetBlackboard(*NodeOwner);
	return BlackboardComp ? BlackboardComp->GetValueAsBool(Key.SelectedKeyName) : false;
}

FString USQFunctionLibrary::GetBlackboardValueAsString(USQNode* NodeOwner, const FBlackboardKeySelector& Key)
{
	check(NodeOwner != NULL);
	const UBlackboardComponent* BlackboardComp = GetBlackboard(*NodeOwner);
	return BlackboardComp ? BlackboardComp->GetValueAsString(Key.SelectedKeyName) : FString();
}

FName USQFunctionLibrary::GetBlackboardValueAsName(USQNode* NodeOwner, const FBlackboardKeySelector& Key)
{
	check(NodeOwner != NULL);
	const UBlackboardComponent* BlackboardComp = GetBlackboard(*NodeOwner);
	return BlackboardComp ? BlackboardComp->GetValueAsName(Key.SelectedKeyName) : NAME_None;
}

FVector USQFunctionLibrary::GetBlackboardValueAsVector(USQNode* NodeOwner, const FBlackboardKeySelector& Key)
{
	check(NodeOwner != NULL);
	const UBlackboardComponent* BlackboardComp = GetBlackboard(*NodeOwner);
	return BlackboardComp ? BlackboardComp->GetValueAsVector(Key.SelectedKeyName) : FVector::ZeroVector;
}

FRotator USQFunctionLibrary::GetBlackboardValueAsRotator(USQNode* NodeOwner, const FBlackboardKeySelector& Key)
{
	check(NodeOwner != NULL);
	const UBlackboardComponent* BlackboardComp = GetBlackboard(*NodeOwner);
	return BlackboardComp ? BlackboardComp->GetValueAsRotator(Key.SelectedKeyName) : FRotator::ZeroRotator;
}

void USQFunctionLibrary::SetBlackboardValueAsObject(USQNode* NodeOwner, const FBlackboardKeySelector& Key, UObject* Value)
{
	check(NodeOwner != NULL);
	UBlackboardComponent* BlackboardComp = GetBlackboard(*NodeOwner);
	if (BlackboardComp != NULL)
	{
		BlackboardComp->SetValue<UBlackboardKeyType_Object>(Key.SelectedKeyName, Value);
	}
}

void USQFunctionLibrary::SetBlackboardValueAsClass(USQNode* NodeOwner, const FBlackboardKeySelector& Key, UClass* Value)
{
	check(NodeOwner != NULL);
	UBlackboardComponent* BlackboardComp = GetBlackboard(*NodeOwner);
	if (BlackboardComp != NULL)
	{
		BlackboardComp->SetValue<UBlackboardKeyType_Class>(Key.SelectedKeyName, Value);
	}
}

void USQFunctionLibrary::SetBlackboardValueAsEnum(USQNode* NodeOwner, const FBlackboardKeySelector& Key, uint8 Value)
{
	check(NodeOwner != NULL);
	UBlackboardComponent* BlackboardComp = GetBlackboard(*NodeOwner);
	if (BlackboardComp != NULL)
	{
		BlackboardComp->SetValue<UBlackboardKeyType_Enum>(Key.SelectedKeyName, Value);
	}
}

void USQFunctionLibrary::SetBlackboardValueAsInt(USQNode* NodeOwner, const FBlackboardKeySelector& Key, int32 Value)
{
	check(NodeOwner != NULL);
	UBlackboardComponent* BlackboardComp = GetBlackboard(*NodeOwner);
	if (BlackboardComp != NULL)
	{
		BlackboardComp->SetValue<UBlackboardKeyType_Int>(Key.SelectedKeyName, Value);
	}
}

void USQFunctionLibrary::SetBlackboardValueAsFloat(USQNode* NodeOwner, const FBlackboardKeySelector& Key, float Value)
{
	check(NodeOwner != NULL);
	UBlackboardComponent* BlackboardComp = GetBlackboard(*NodeOwner);
	if (BlackboardComp != NULL)
	{
		BlackboardComp->SetValue<UBlackboardKeyType_Float>(Key.SelectedKeyName, Value);
	}
}

void USQFunctionLibrary::SetBlackboardValueAsBool(USQNode* NodeOwner, const FBlackboardKeySelector& Key, bool Value)
{
	check(NodeOwner != NULL);
	UBlackboardComponent* BlackboardComp = GetBlackboard(*NodeOwner);
	if (BlackboardComp != NULL)
	{
		BlackboardComp->SetValue<UBlackboardKeyType_Bool>(Key.SelectedKeyName, Value);
	}
}

void USQFunctionLibrary::SetBlackboardValueAsString(USQNode* NodeOwner, const FBlackboardKeySelector& Key, FString Value)
{
	check(NodeOwner != NULL);
	UBlackboardComponent* BlackboardComp = GetBlackboard(*NodeOwner);
	if (BlackboardComp != NULL)
	{
		BlackboardComp->SetValue<UBlackboardKeyType_String>(Key.SelectedKeyName, Value);
	}
}

void USQFunctionLibrary::SetBlackboardValueAsName(USQNode* NodeOwner, const FBlackboardKeySelector& Key, FName Value)
{
	check(NodeOwner != NULL);
	UBlackboardComponent* BlackboardComp = GetBlackboard(*NodeOwner);
	if (BlackboardComp != NULL)
	{
		BlackboardComp->SetValue<UBlackboardKeyType_Name>(Key.SelectedKeyName, Value);
	}
}

void USQFunctionLibrary::SetBlackboardValueAsVector(USQNode* NodeOwner, const FBlackboardKeySelector& Key, FVector Value)
{
	check(NodeOwner != NULL);
	UBlackboardComponent* BlackboardComp = GetBlackboard(*NodeOwner);
	if (BlackboardComp != NULL)
	{
		BlackboardComp->SetValue<UBlackboardKeyType_Vector>(Key.SelectedKeyName, Value);
	}
}

void USQFunctionLibrary::ClearBlackboardValueAsVector(USQNode* NodeOwner, const FBlackboardKeySelector& Key)
{
	check(NodeOwner != NULL);
	UBlackboardComponent* BlackboardComp = GetBlackboard(*NodeOwner);
	if (BlackboardComp != NULL)
	{
		BlackboardComp->ClearValue(Key.SelectedKeyName);
	}
}

void USQFunctionLibrary::SetBlackboardValueAsRotator(USQNode* NodeOwner, const FBlackboardKeySelector& Key, FRotator Value)
{
	check(NodeOwner != NULL);
	UBlackboardComponent* BlackboardComp = GetBlackboard(*NodeOwner);
	if (BlackboardComp != NULL)
	{
		BlackboardComp->SetValue<UBlackboardKeyType_Rotator>(Key.SelectedKeyName, Value);
	}
}

void USQFunctionLibrary::ClearBlackboardValue(USQNode* NodeOwner, const FBlackboardKeySelector& Key)
{
	check(NodeOwner != NULL);
	UBlackboardComponent* BlackboardComp = GetBlackboard(*NodeOwner);
	if (BlackboardComp != NULL)
	{
		BlackboardComp->ClearValue(Key.SelectedKeyName);
	}
}

void USQFunctionLibrary::StartUsingExternalEvent(USQNode* NodeOwner, AActor* OwningActor)
{
	// deprecated, not removed yet
}

void USQFunctionLibrary::StopUsingExternalEvent(USQNode* NodeOwner)
{
	// deprecated, not removed yet
}
