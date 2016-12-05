// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"
#include "SelectionQuery.generated.h"

class USQCompositeNode;

/**
 * 
 */
UCLASS()
class SELECTIONQUERY_API USelectionQuery : public UObject
{
    GENERATED_UCLASS_BODY()

    /** root node of loaded tree */
    UPROPERTY()
    USQCompositeNode* RootNode;

#if WITH_EDITORONLY_DATA
    /** Graph for query */
    UPROPERTY()
    UEdGraph*	EdGraph;
#endif

protected:
    friend class USelectionQueryManager;

    UPROPERTY()
    FName QueryName;

public:
    virtual  void PostInitProperties() override;

    /** QueryName patching up */
    virtual void PostLoad() override;
#if WITH_EDITOR
    virtual void PostDuplicate(bool bDuplicateForPIE) override;
#endif // WITH_EDITOR

    FName GetQueryName() const { return QueryName; }
};
