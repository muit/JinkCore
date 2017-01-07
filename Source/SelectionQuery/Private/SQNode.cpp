// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "SelectionQueryPrivatePCH.h"
#include "SelectionQuery.h"
#include "SQNode.h"
#include "SQCompositeNode.h"
#include "GameplayTasksComponent.h"

//----------------------------------------------------------------------//
// USQNode
//----------------------------------------------------------------------//

USQNode::USQNode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    ParentNode = NULL;
    QueryAsset = NULL;

#if USE_SelectionQuery_DEBUGGER
    NextExecutionNode = NULL;
#endif
}

/*
void USQNode::OnInstanceCreated(USelectionQueryComponent& OwnerComp) { //empty in base class }
void USQNode::OnInstanceDestroyed(USelectionQueryComponent& OwnerComp) { // empty in base class }
*/

void USQNode::InitializeNode(USQCompositeNode * InParentNode, uint16 InExecutionIndex, uint8 InGraphDepth)
{
    ParentNode = InParentNode;
    ExecutionIndex = InExecutionIndex;
    GraphDepth = InGraphDepth;
}

void USQNode::InitializeFromAsset(USelectionQuery & Asset)
{
    QueryAsset = &Asset;
}

FString USQNode::GetStaticDescription() const
{
    // short type name
    return "";//USelectionQueryTypes::GetShortTypeName(this);
}

#if WITH_EDITOR
FName USQNode::GetNodeIconName() const
{
    return NAME_None;
}

bool USQNode::UsesBlueprint() const
{
    return false;
}
#endif

FString USQNode::GetNodeName() const
{
    return NodeName/*.Len() ? NodeName : USelectionQueryTypes::GetShortTypeName(this)*/;
}
