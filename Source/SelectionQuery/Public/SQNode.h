// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

//#include "SelectionQueryTypes.h"
#include "GameplayTaskOwnerInterface.h"
#include "SQNode.generated.h"

class USelectionQuery;
class USQCompositeNode;

UCLASS(Abstract)
class SELECTIONQUERY_API USQNode : public UObject
{
    GENERATED_UCLASS_BODY()

    /** Versioning for updating deprecated properties */
    UPROPERTY()
    int32 VerNum;

    /** node name */
    UPROPERTY(Category = Description, EditAnywhere)
    FString NodeName;


    /** fill in data about tree structure */
    void InitializeNode(USQCompositeNode* InParentNode, uint16 InExecutionIndex, uint8 InGraphDepth);

    /** initialize any asset related data */
    virtual void InitializeFromAsset(USelectionQuery& Asset);

private:

    /** source asset */
    UPROPERTY()
    USelectionQuery* QueryAsset;

    /** parent node */
    UPROPERTY()
    USQCompositeNode* ParentNode;

    /** depth in tree */
    uint8 GraphDepth;

    uint16 ExecutionIndex;

public:
    /** @return string containing description of this node with all setup values */
    virtual FString GetStaticDescription() const;

#if WITH_EDITOR
    /** Get the name of the icon used to display this node in the editor */
    virtual FName GetNodeIconName() const;

    /** Get whether this node is using a blueprint for its logic */
    virtual bool UsesBlueprint() const;
#endif

    /** @return name of node */
    FString GetNodeName() const;


    virtual UObject* GetThumbnailAssetObject() { return nullptr; }
};
