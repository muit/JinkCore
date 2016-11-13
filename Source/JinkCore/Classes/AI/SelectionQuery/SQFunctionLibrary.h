// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "SQFunctionLibrary.generated.h"

class UBlackboardComponent;
struct FBlackboardKeySelector;

namespace FSQNodeBPImplementationHelper
{
	static const int32 NoImplementation = 0;
	static const int32 Generic = 1 << 0;
	static const int32 AISpecific = 1 << 1;
	static const int32 All = Generic | AISpecific;

	/** checks if given object implements GenericEventName and/or AIEventName BP events, and returns an result as flags set on return integer 
	 *	@return flags set in returned integer indicate kinds of events implemented by given object */
	AIMODULE_API int32 CheckEventImplementationVersion(FName GenericEventName, FName AIEventName, const UObject& Object, const UClass& StopAtClass);

	DEPRECATED(4.11, "This version of CheckEventImplementationVersion is deprecated. Please use the one taking reference to UObject and StopAtClass rather than a pointers.")
	AIMODULE_API int32 CheckEventImplementationVersion(FName GenericEventName, FName AIEventName, const UObject* Ob, const UClass* StopAtClass);
}

UCLASS(meta=(RestrictedToClasses="SQNode"))
class AIMODULE_API USQFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category="AI|SelectionQuery", Meta=(HidePin="NodeOwner", DefaultToSelf="NodeOwner"))
	static UBlackboardComponent* GetOwnersBlackboard(USQNode* NodeOwner);

	UFUNCTION(BlueprintPure, Category = "AI|SelectionQuery", Meta=(HidePin="NodeOwner", DefaultToSelf="NodeOwner"))
	static USelectionQueryComponent* GetOwnerComponent(USQNode* NodeOwner);

	UFUNCTION(BlueprintPure, Category="AI|SelectionQuery", Meta=(HidePin="NodeOwner", DefaultToSelf="NodeOwner"))
	static UObject* GetBlackboardValueAsObject(USQNode* NodeOwner, const FBlackboardKeySelector& Key);

	UFUNCTION(BlueprintPure, Category="AI|SelectionQuery", Meta=(HidePin="NodeOwner", DefaultToSelf="NodeOwner"))
	static AActor* GetBlackboardValueAsActor(USQNode* NodeOwner, const FBlackboardKeySelector& Key);

	UFUNCTION(BlueprintPure, Category="AI|SelectionQuery", Meta=(HidePin="NodeOwner", DefaultToSelf="NodeOwner"))
	static UClass* GetBlackboardValueAsClass(USQNode* NodeOwner, const FBlackboardKeySelector& Key);

	UFUNCTION(BlueprintPure, Category="AI|SelectionQuery", Meta=(HidePin="NodeOwner", DefaultToSelf="NodeOwner"))
	static uint8 GetBlackboardValueAsEnum(USQNode* NodeOwner, const FBlackboardKeySelector& Key);

	UFUNCTION(BlueprintPure, Category="AI|SelectionQuery", Meta=(HidePin="NodeOwner", DefaultToSelf="NodeOwner"))
	static int32 GetBlackboardValueAsInt(USQNode* NodeOwner, const FBlackboardKeySelector& Key);

	UFUNCTION(BlueprintPure, Category="AI|SelectionQuery", Meta=(HidePin="NodeOwner", DefaultToSelf="NodeOwner"))
	static float GetBlackboardValueAsFloat(USQNode* NodeOwner, const FBlackboardKeySelector& Key);

	UFUNCTION(BlueprintPure, Category="AI|SelectionQuery", Meta=(HidePin="NodeOwner", DefaultToSelf="NodeOwner"))
	static bool GetBlackboardValueAsBool(USQNode* NodeOwner, const FBlackboardKeySelector& Key);

	UFUNCTION(BlueprintPure, Category="AI|SelectionQuery", Meta=(HidePin="NodeOwner", DefaultToSelf="NodeOwner"))
	static FString GetBlackboardValueAsString(USQNode* NodeOwner, const FBlackboardKeySelector& Key);

	UFUNCTION(BlueprintPure, Category="AI|SelectionQuery", Meta=(HidePin="NodeOwner", DefaultToSelf="NodeOwner"))
	static FName GetBlackboardValueAsName(USQNode* NodeOwner, const FBlackboardKeySelector& Key);

	UFUNCTION(BlueprintPure, Category="AI|SelectionQuery", Meta=(HidePin="NodeOwner", DefaultToSelf="NodeOwner"))
	static FVector GetBlackboardValueAsVector(USQNode* NodeOwner, const FBlackboardKeySelector& Key);

	UFUNCTION(BlueprintPure, Category ="AI|SelectionQuery", Meta=(HidePin="NodeOwner", DefaultToSelf="NodeOwner"))
	static FRotator GetBlackboardValueAsRotator(USQNode* NodeOwner, const FBlackboardKeySelector& Key);

	UFUNCTION(BlueprintCallable, Category="AI|SelectionQuery", Meta=(HidePin="NodeOwner", DefaultToSelf="NodeOwner"))
	static void SetBlackboardValueAsObject(USQNode* NodeOwner, const FBlackboardKeySelector& Key, UObject* Value);

	UFUNCTION(BlueprintCallable, Category="AI|SelectionQuery", Meta=(HidePin="NodeOwner", DefaultToSelf="NodeOwner"))
	static void SetBlackboardValueAsClass(USQNode* NodeOwner, const FBlackboardKeySelector& Key, UClass* Value);

	UFUNCTION(BlueprintCallable, Category="AI|SelectionQuery", Meta=(HidePin="NodeOwner", DefaultToSelf="NodeOwner"))
	static void SetBlackboardValueAsEnum(USQNode* NodeOwner, const FBlackboardKeySelector& Key, uint8 Value);

	UFUNCTION(BlueprintCallable, Category="AI|SelectionQuery", Meta=(HidePin="NodeOwner", DefaultToSelf="NodeOwner"))
	static void SetBlackboardValueAsInt(USQNode* NodeOwner, const FBlackboardKeySelector& Key, int32 Value);

	UFUNCTION(BlueprintCallable, Category="AI|SelectionQuery", Meta=(HidePin="NodeOwner", DefaultToSelf="NodeOwner"))
	static void SetBlackboardValueAsFloat(USQNode* NodeOwner, const FBlackboardKeySelector& Key, float Value);

	UFUNCTION(BlueprintCallable, Category="AI|SelectionQuery", Meta=(HidePin="NodeOwner", DefaultToSelf="NodeOwner"))
	static void SetBlackboardValueAsBool(USQNode* NodeOwner, const FBlackboardKeySelector& Key, bool Value);

	UFUNCTION(BlueprintCallable, Category="AI|SelectionQuery", Meta=(HidePin="NodeOwner", DefaultToSelf="NodeOwner"))
	static void SetBlackboardValueAsString(USQNode* NodeOwner, const FBlackboardKeySelector& Key, FString Value);

	UFUNCTION(BlueprintCallable, Category="AI|SelectionQuery", Meta=(HidePin="NodeOwner", DefaultToSelf="NodeOwner"))
	static void SetBlackboardValueAsName(USQNode* NodeOwner, const FBlackboardKeySelector& Key, FName Value);

	UFUNCTION(BlueprintCallable, Category="AI|SelectionQuery", Meta=(HidePin="NodeOwner", DefaultToSelf="NodeOwner"))
	static void SetBlackboardValueAsVector(USQNode* NodeOwner, const FBlackboardKeySelector& Key, FVector Value);

	/** (DEPRECATED) Use ClearBlackboardValue instead */
	UFUNCTION(BlueprintCallable, Category=SelectionQuery, Meta=(HidePin="NodeOwner", DefaultToSelf="NodeOwner", DeprecatedFunction, DeprecationMessage="Use ClearBlackboardValue instead."))
	static void ClearBlackboardValueAsVector(USQNode* NodeOwner, const FBlackboardKeySelector& Key);

	UFUNCTION(BlueprintCallable, Category = "AI|SelectionQuery", Meta = (HidePin = "NodeOwner", DefaultToSelf = "NodeOwner"))
	static void SetBlackboardValueAsRotator(USQNode* NodeOwner, const FBlackboardKeySelector& Key, FRotator Value);

	/** Resets indicated value to "not set" value, based on values type */
	UFUNCTION(BlueprintCallable, Category = SelectionQuery, Meta = (HidePin = "NodeOwner", DefaultToSelf = "NodeOwner"))
	static void ClearBlackboardValue(USQNode* NodeOwner, const FBlackboardKeySelector& Key);

	/** Initialize variables marked as "instance memory" and set owning actor for blackboard operations */
	UFUNCTION(BlueprintCallable, Category="AI|SelectionQuery", Meta=(HidePin="NodeOwner", DefaultToSelf="NodeOwner", DeprecatedFunction, DeprecationMessage="No longer needed"))
	static void StartUsingExternalEvent(USQNode* NodeOwner, AActor* OwningActor);

	/** Save variables marked as "instance memory" and clear owning actor */
	UFUNCTION(BlueprintCallable, Category="AI|SelectionQuery", Meta=(HidePin="NodeOwner", DefaultToSelf="NodeOwner", DeprecatedFunction, DeprecationMessage="No longer needed"))
	static void StopUsingExternalEvent(USQNode* NodeOwner);
};
