// Copyright 2015-2017 Piperift. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "GameFramework/Actor.h"
#include "EntityAttribute.h"

#include "AttributesFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class JINKCORE_API UAttributesFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

    // Compare two Attributes by Guid
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Attributes")
    static bool Is(UPARAM(ref) FEntityAttribute& A, UPARAM(ref) FEntityAttribute& B);

    // Compare two Attributes by Guid
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Attributes")
    static bool Equals(UPARAM(ref) FEntityAttribute& A, UPARAM(ref) FEntityAttribute& B);

    //Get final value
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Attributes")
    static const float GetValue (const FEntityAttribute& Attribute);

    //Get final value
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Attributes")
    static const float ToFloat (const FEntityAttribute& Attribute);
	
    //Get base value
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Attributes")
    static const float& GetBase(const FEntityAttribute& Attribute);
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Attributes")
    void SetBase(UPARAM(ref) FEntityAttribute & Attribute, float Value);

    UFUNCTION(BlueprintCallable, Category = "Attributes")
    static void AddModification(UPARAM(ref) FEntityAttribute& Attribute, UPARAM(ref) FAttributeModification& Modification);
    //static void AddLinkedModification(FEntityAttribute& Attribute, FAttributeModification& Modification, AActor* Owner);

    UFUNCTION(BlueprintCallable, Category = "Attributes")
    static void RemoveModification(UPARAM(ref) FEntityAttribute& Attribute, UPARAM(ref) FAttributeModification& Modification);
};
