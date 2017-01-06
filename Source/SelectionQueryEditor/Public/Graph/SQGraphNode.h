// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "EdGraph/EdGraphNode.h"
#include "EdGraph_SelectionQuery.h"

#include "SQGraphNode.generated.h"

struct FGraphNodeClassData;

class SELECTIONQUERYEDITOR_API FNodePropertyObserver {
public:
    virtual void OnPropertyChanged(class USQGraphNode* Sender, const FName& PropertyName) = 0;
};

UCLASS()
class SELECTIONQUERYEDITOR_API USQGraphNode : public UEdGraphNode
{
    GENERATED_UCLASS_BODY()

    /** instance class */
    UPROPERTY()
    struct FGraphNodeClassData ClassData;

    UPROPERTY()
    UObject* NodeInstance;

    /** if set, all modifications (including delete/cut) are disabled */
    UPROPERTY()
    FString NodeName;

    UPROPERTY()
    uint32 bIsReadOnly : 1;

    void InitializeInstance();

    //~ Begin UEdGraphNode Interface
    virtual void PostPlacedNewNode() override;
    virtual bool CanDuplicateNode() const override;
    virtual bool CanUserDeleteNode() const override;
    //~ End UEdGraphNode Interface


    virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
    virtual FText GetDescription() const;

    UEdGraph_SelectionQuery* GetSQGraph();

    virtual bool CanCreateUnderSpecifiedSchema(const UEdGraphSchema* DesiredSchema) const override;

public:
    virtual void PostEditChangeProperty(struct FPropertyChangedEvent& e);
    virtual UEdGraphPin* GetInputPin() const { return nullptr; }
    virtual UEdGraphPin* GetOutputPin() const { return nullptr; }
    virtual void NodeConnectionListChanged() override;

    TSharedPtr<FNodePropertyObserver> PropertyObserver;
};
