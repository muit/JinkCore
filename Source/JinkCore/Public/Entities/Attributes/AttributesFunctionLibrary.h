// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "EntityAttribute.h"

#include "AttributesFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class JINKCORE_API UAttributesFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
    
    //Get final value
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Attributes")
    static const float GetValue (const FEntityAttribute& Attribute);

    //Get final value
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Attributes")
    static const float ToFloat (const FEntityAttribute& Attribute);
	
    //Get base value
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Attributes")
    static const float& GetBase (const FEntityAttribute& Attribute);
};
