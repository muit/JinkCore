// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "JinkCorePrivatePCH.h"
#include "AI/SelectionQuery/SQCompositeNode.h"
#include "AI/SelectionQuery/SelectionQueryTypes.h"

//----------------------------------------------------------------------//
// FSelectionQueryInstance
//----------------------------------------------------------------------//
void FSelectionQueryInstance::Initialize(USelectionQueryComponent& OwnerComp, UBTCompositeNode& Node, int32& InstancedIndex)
{
	Node.InitializeInSubtree(OwnerComp, InstancedIndex, InitType);

	USQCompositeNode* InstancedComposite = Cast<USQCompositeNode>(Node.GetNodeInstance(OwnerComp, NodeMemory));
	if (InstancedComposite)
	{
		InstancedComposite->InitializeComposite();
	}

	for (int32 ChildIndex = 0; ChildIndex < Node.Children.Num(); ChildIndex++)
	{
		FBTCompositeChild& ChildInfo = Node.Children[ChildIndex];

		if (ChildInfo.ChildComposite)
		{
			Initialize(OwnerComp, *(ChildInfo.ChildComposite), InstancedIndex, InitType);
		}
		else if (ChildInfo.ChildTask)
		{
			ChildInfo.ChildTask->InitializeInSubtree(OwnerComp, InstancedIndex, InitType);
		}
	}
}

void FSelectionQueryInstance::Cleanup(USelectionQueryComponent& OwnerComp, EBTMemoryClear::Type CleanupType)
{
	FSelectionQueryInstanceId& Info = OwnerComp.KnownInstances[InstanceIdIndex];
	if (Info.FirstNodeInstance >= 0)
	{
		const int32 MaxAllowedIdx = OwnerComp.NodeInstances.Num();
		const int32 LastNodeIdx = OwnerComp.KnownInstances.IsValidIndex(InstanceIdIndex + 1) ?
			FMath::Min(OwnerComp.KnownInstances[InstanceIdIndex + 1].FirstNodeInstance, MaxAllowedIdx) :
			MaxAllowedIdx;

		for (int32 Idx = Info.FirstNodeInstance; Idx < LastNodeIdx; Idx++)
		{
			OwnerComp.NodeInstances[Idx]->OnInstanceDestroyed(OwnerComp);
		}
	}

	CleanupNodes(OwnerComp, *RootNode, CleanupType);

	// remove memory when instance is destroyed - it will need full initialize anyway
	if (CleanupType == EBTMemoryClear::Destroy)
	{
		Info.InstanceMemory.Empty();
	}
	else
	{
		Info.InstanceMemory = InstanceMemory;
	}
}

void FSelectionQueryInstance::CleanupNodes(USelectionQueryComponent& OwnerComp, UBTCompositeNode& Node, EBTMemoryClear::Type CleanupType)
{
	for (int32 ServiceIndex = 0; ServiceIndex < Node.Services.Num(); ServiceIndex++)
	{
		Node.Services[ServiceIndex]->CleanupInSubtree(OwnerComp, Node.Services[ServiceIndex]->GetNodeMemory<uint8>(*this), CleanupType);
	}

	Node.CleanupInSubtree(OwnerComp, Node.GetNodeMemory<uint8>(*this), CleanupType);

	for (int32 ChildIndex = 0; ChildIndex < Node.Children.Num(); ChildIndex++)
	{
		FBTCompositeChild& ChildInfo = Node.Children[ChildIndex];

		for (int32 DecoratorIndex = 0; DecoratorIndex < ChildInfo.Decorators.Num(); DecoratorIndex++)
		{
			ChildInfo.Decorators[DecoratorIndex]->CleanupInSubtree(OwnerComp, ChildInfo.Decorators[DecoratorIndex]->GetNodeMemory<uint8>(*this), CleanupType);
		}

		if (ChildInfo.ChildComposite)
		{
			CleanupNodes(OwnerComp, *(ChildInfo.ChildComposite), CleanupType);
		}
		else if (ChildInfo.ChildTask)
		{
			for (int32 ServiceIndex = 0; ServiceIndex < ChildInfo.ChildTask->Services.Num(); ServiceIndex++)
			{
				ChildInfo.ChildTask->Services[ServiceIndex]->CleanupInSubtree(OwnerComp, ChildInfo.ChildTask->Services[ServiceIndex]->GetNodeMemory<uint8>(*this), CleanupType);
			}

			ChildInfo.ChildTask->CleanupInSubtree(OwnerComp, ChildInfo.ChildTask->GetNodeMemory<uint8>(*this), CleanupType);
		}
	}
}

bool FSelectionQueryInstance::HasActiveNode(uint16 TestExecutionIndex) const
{
	if (ActiveNode && ActiveNode->GetExecutionIndex() == TestExecutionIndex)
	{
		return true;
	}

	for (int32 Idx = 0; Idx < ParallelTasks.Num(); Idx++)
	{
		const FSelectionQueryParallelTask& ParallelTask = ParallelTasks[Idx];
		if (ParallelTask.TaskNode && ParallelTask.TaskNode->GetExecutionIndex() == TestExecutionIndex)
		{
			return (ParallelTask.Status == EBTTaskStatus::Active);
		}
	}

	for (int32 Idx = 0; Idx < ActiveAuxNodes.Num(); Idx++)
	{
		if (ActiveAuxNodes[Idx] && ActiveAuxNodes[Idx]->GetExecutionIndex() == TestExecutionIndex)
		{
			return true;
		}
	}

	return false;
}

void FSelectionQueryInstance::DeactivateNodes(FSelectionQuerySearchData& SearchData, uint16 InstanceIndex)
{
	for (int32 Idx = SearchData.PendingUpdates.Num() - 1; Idx >= 0; Idx--)
	{
		FSelectionQuerySearchUpdate& UpdateInfo = SearchData.PendingUpdates[Idx];
		if (UpdateInfo.InstanceIndex == InstanceIndex && UpdateInfo.Mode == EBTNodeUpdateMode::Add)
		{
			UE_VLOG(SearchData.OwnerComp.GetOwner(), LogSelectionQuery, Verbose, TEXT("Search node update[%s]: %s"),
				*USelectionQueryTypes::DescribeNodeUpdateMode(EBTNodeUpdateMode::Remove),
				*USelectionQueryTypes::DescribeNodeHelper(UpdateInfo.AuxNode ? (UBTNode*)UpdateInfo.AuxNode : (UBTNode*)UpdateInfo.TaskNode));

			SearchData.PendingUpdates.RemoveAt(Idx, 1, false);
		}
	}

	for (int32 Idx = 0; Idx < ParallelTasks.Num(); Idx++)
	{
		const FSelectionQueryParallelTask& ParallelTask = ParallelTasks[Idx];
		if (ParallelTask.TaskNode && ParallelTask.Status == EBTTaskStatus::Active)
		{
			SearchData.AddUniqueUpdate(FSelectionQuerySearchUpdate(ParallelTask.TaskNode, InstanceIndex, EBTNodeUpdateMode::Remove));
		}
	}

	for (int32 Idx = 0; Idx < ActiveAuxNodes.Num(); Idx++)
	{
		if (ActiveAuxNodes[Idx])
		{
			SearchData.AddUniqueUpdate(FSelectionQuerySearchUpdate(ActiveAuxNodes[Idx], InstanceIndex, EBTNodeUpdateMode::Remove));
		}
	}
}


//----------------------------------------------------------------------//
// FBTNodeIndex
//----------------------------------------------------------------------//

bool FBTNodeIndex::TakesPriorityOver(const FBTNodeIndex& Other) const
{
	// instance closer to root is more important
	if (InstanceIndex != Other.InstanceIndex)
	{
		return InstanceIndex < Other.InstanceIndex;
	}

	// higher priority is more important
	return ExecutionIndex < Other.ExecutionIndex;
}


//----------------------------------------------------------------------//
// USelectionQueryTypes
//----------------------------------------------------------------------//
USelectionQueryTypes::USelectionQueryTypes(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

FString USelectionQueryTypes::DescribeNodeHelper(const USQNode* Node)
{
	return Node ? FString::Printf(TEXT("%s"), *Node->GetNodeName()) : FString();
}

FString USelectionQueryTypes::GetShortTypeName(const UObject* Ob)
{
	if (Ob->GetClass()->HasAnyClassFlags(CLASS_CompiledFromBlueprint))
	{
		return Ob->GetClass()->GetName().LeftChop(2);
	}

	FString TypeDesc = Ob->GetClass()->GetName();
	const int32 ShortNameIdx = TypeDesc.Find(TEXT("_"));
	if (ShortNameIdx != INDEX_NONE)
	{
		TypeDesc = TypeDesc.Mid(ShortNameIdx + 1);
	}

	return TypeDesc;
}
