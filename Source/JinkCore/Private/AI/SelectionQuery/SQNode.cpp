// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "JinkCorePrivatePCH.h"
#include "SelectionQuery/SelectionQuery.h"
#include "SelectionQuery/SQNode.h"
#include "SelectionQuery/SQCompositeNode.h"
#include "GameplayTasksComponent.h"

//----------------------------------------------------------------------//
// USQNode
//----------------------------------------------------------------------//

USQNode::USQNode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	ParentNode = NULL;
	TreeAsset = NULL;
	ExecutionIndex = 0;
	MemoryOffset = 0;
	TreeDepth = 0;
	bCreateNodeInstance = false;
	bIsInstanced = false;
	bIsInjected = false;

#if USE_SelectionQuery_DEBUGGER
	NextExecutionNode = NULL;
#endif
}

UWorld* USQNode::GetWorld() const
{
	// instanced nodes are created for behavior tree component owning that instance
	// template nodes are created for behavior tree manager, which is located directly in UWorld

	return GetOuter() == NULL ? NULL :
		IsInstanced() ? (Cast<USelectionQueryComponent>(GetOuter()))->GetWorld() :
		Cast<UWorld>(GetOuter()->GetOuter());
}

void USQNode::InitializeNode(USQCompositeNode* InParentNode, uint16 InExecutionIndex, uint16 InMemoryOffset, uint8 InTreeDepth)
{
	ParentNode = InParentNode;
	ExecutionIndex = InExecutionIndex;
	MemoryOffset = InMemoryOffset;
	TreeDepth = InTreeDepth;
}

void USQNode::InitializeMemory(USelectionQueryComponent& OwnerComp, uint8* NodeMemory, ESQMemoryInit::Type InitType) const
{
	// empty in base 
}

void USQNode::CleanupMemory(USelectionQueryComponent& OwnerComp, uint8* NodeMemory, ESQMemoryClear::Type CleanupType) const
{
	// empty in base 
}

void USQNode::OnInstanceCreated(USelectionQueryComponent& OwnerComp)
{
	// empty in base class
}

void USQNode::OnInstanceDestroyed(USelectionQueryComponent& OwnerComp)
{
	// empty in base class
}

void USQNode::InitializeInSuSQree(USelectionQueryComponent& OwnerComp, uint8* NodeMemory, int32& NextInstancedIndex, ESQMemoryInit::Type InitType) const
{
	FSQInstancedNodeMemory* SpecialMemory = GetSpecialNodeMemory<FSQInstancedNodeMemory>(NodeMemory);
	if (SpecialMemory)
	{
		SpecialMemory->NodeIdx = INDEX_NONE;
	}

	if (bCreateNodeInstance)
	{
		// composite nodes can't be instanced!
		check(IsA(USQCompositeNode::StaticClass()) == false);

		USQNode* NodeInstance = OwnerComp.NodeInstances.IsValidIndex(NextInstancedIndex) ? OwnerComp.NodeInstances[NextInstancedIndex] : NULL;
		if (NodeInstance == NULL)
		{
			NodeInstance = (USQNode*)StaticDuplicateObject(this, &OwnerComp);
			NodeInstance->InitializeNode(GetParentNode(), GetExecutionIndex(), GetMemoryOffset(), GetTreeDepth());
			NodeInstance->bIsInstanced = true;

			OwnerComp.NodeInstances.Add(NodeInstance);
		}

		check(NodeInstance);
		check(SpecialMemory);

		SpecialMemory->NodeIdx = NextInstancedIndex;

		NodeInstance->SetOwner(OwnerComp.GetOwner());
		NodeInstance->InitializeMemory(OwnerComp, NodeMemory, InitType);
		check(TreeAsset);
		NodeInstance->InitializeFromAsset(*TreeAsset);
		NodeInstance->OnInstanceCreated(OwnerComp);
		NextInstancedIndex++;
	}
	else
	{
		InitializeMemory(OwnerComp, NodeMemory, InitType);
	}
}

void USQNode::CleanupInSuSQree(USelectionQueryComponent& OwnerComp, uint8* NodeMemory, ESQMemoryClear::Type CleanupType) const
{
	const USQNode* NodeOb = bCreateNodeInstance ? GetNodeInstance(OwnerComp, NodeMemory) : this;
	if (NodeOb)
	{
		NodeOb->CleanupMemory(OwnerComp, NodeMemory, CleanupType);
	}
}

#if USE_SelectionQuery_DEBUGGER
void USQNode::InitializeExecutionOrder(USQNode* NextNode)
{
	NextExecutionNode = NextNode;
}
#endif

void USQNode::InitializeFromAsset(USelectionQuery& Asset)
{
	TreeAsset = &Asset;
}

UBlackboardData* USQNode::GetBlackboardAsset() const
{
	return TreeAsset ? TreeAsset->BlackboardAsset : NULL;
}

uint16 USQNode::GetInstanceMemorySize() const
{
	return 0;
}

uint16 USQNode::GetSpecialMemorySize() const
{
	return bCreateNodeInstance ? sizeof(FSQInstancedNodeMemory) : 0;
}

USQNode* USQNode::GetNodeInstance(const USelectionQueryComponent& OwnerComp, uint8* NodeMemory) const
{
	FSQInstancedNodeMemory* MyMemory = GetSpecialNodeMemory<FSQInstancedNodeMemory>(NodeMemory);
	return MyMemory && OwnerComp.NodeInstances.IsValidIndex(MyMemory->NodeIdx) ?
		OwnerComp.NodeInstances[MyMemory->NodeIdx] : NULL;
}

USQNode* USQNode::GetNodeInstance(FSelectionQuerySearchData& SearchData) const
{
	return GetNodeInstance(SearchData.OwnerComp, GetNodeMemory<uint8>(SearchData));
}

FString USQNode::GetNodeName() const
{
	return NodeName.Len() ? NodeName : USelectionQueryTypes::GetShortTypeName(this);
}

FString USQNode::GetRuntimeDescription(const USelectionQueryComponent& OwnerComp, uint8* NodeMemory, ESQDescriptionVerbosity::Type Verbosity) const
{
	FString Description = NodeName.Len() ? FString::Printf(TEXT("%d. %s [%s]"), ExecutionIndex, *NodeName, *GetStaticDescription()) : GetStaticDescription();
	TArray<FString> RuntimeValues;

	const USQNode* NodeOb = bCreateNodeInstance ? GetNodeInstance(OwnerComp, NodeMemory) : this;
	if (NodeOb)
	{
		NodeOb->DescribeRuntimeValues(OwnerComp, NodeMemory, Verbosity, RuntimeValues);
	}

	for (int32 ValueIndex = 0; ValueIndex < RuntimeValues.Num(); ValueIndex++)
	{
		Description += TEXT(", ");
		Description += RuntimeValues[ValueIndex];
	}

	return Description;
}

FString USQNode::GetStaticDescription() const
{
	// short type name
	return USelectionQueryTypes::GetShortTypeName(this);
}

void USQNode::DescribeRuntimeValues(const USelectionQueryComponent& OwnerComp, uint8* NodeMemory, ESQDescriptionVerbosity::Type Verbosity, TArray<FString>& Values) const
{
	// nothing stored in memory for base class
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

UGameplayTasksComponent* USQNode::GetGameplayTasksComponent(const UGameplayTask& Task) const
{
	const UAITask* AITask = Cast<const UAITask>(&Task);
	return (AITask && AITask->GetAIController()) ? AITask->GetAIController()->GetGameplayTasksComponent(Task) : Task.GetGameplayTasksComponent();
}

AActor* USQNode::GetGameplayTaskOwner(const UGameplayTask* Task) const
{
	if (Task == nullptr)
	{
		if (IsInstanced())
		{
			const USelectionQueryComponent* SQComponent = Cast<const USelectionQueryComponent>(GetOuter());
			//not having SQ component for an instanced SQ node is invalid!
			check(SQComponent);
			return SQComponent->GetAIOwner();
		}
		else
		{
			UE_LOG(LogSelectionQuery, Warning, TEXT("%s: Unable to determine default GameplayTaskOwner!"), *GetName());
			return nullptr;
		}
	}

	const UAITask* AITask = Cast<const UAITask>(Task);
	if (AITask)
	{
		return AITask->GetAIController();
	}

	const UGameplayTasksComponent* TasksComponent = Task->GetGameplayTasksComponent();
	return TasksComponent ? TasksComponent->GetGameplayTaskOwner(Task) : nullptr;
}

AActor* USQNode::GetGameplayTaskAvatar(const UGameplayTask* Task) const
{
	if (Task == nullptr)
	{
		if (IsInstanced())
		{
			const USelectionQueryComponent* SQComponent = Cast<const USelectionQueryComponent>(GetOuter());
			//not having SQ component for an instanced SQ node is invalid!
			check(SQComponent);
			return SQComponent->GetAIOwner();
		}
		else
		{
			UE_LOG(LogSelectionQuery, Warning, TEXT("%s: Unable to determine default GameplayTaskAvatar!"), *GetName());
			return nullptr;
		}
	}

	const UAITask* AITask = Cast<const UAITask>(Task);
	if (AITask)
	{
		return AITask->GetAIController() ? AITask->GetAIController()->GetPawn() : nullptr;
	}

	const UGameplayTasksComponent* TasksComponent = Task->GetGameplayTasksComponent();
	return TasksComponent ? TasksComponent->GetGameplayTaskAvatar(Task) : nullptr;
}

uint8 USQNode::GetGameplayTaskDefaultPriority() const
{
	return static_cast<uint8>(EAITaskPriority::AutonomousAI);
}

void USQNode::OnGameplayTaskInitialized(UGameplayTask& Task)
{
	const UAITask* AITask = Cast<const UAITask>(&Task);
	if (AITask && (AITask->GetAIController() == nullptr))
	{
		// this means that the task has either been created without specifying 
		// UAITAsk::OwnerController's value (like via BP's Construct Object node)
		// or it has been created in C++ with inappropriate function
		UE_LOG(LogSelectionQuery, Error, TEXT("Missing AIController in AITask %s"), *AITask->GetName());
	}
}

USelectionQueryComponent* USQNode::GetSQComponentForTask(UGameplayTask& Task) const
{
	UAITask* AITask = Cast<UAITask>(&Task);
	return (AITask && AITask->GetAIController()) ? Cast<USelectionQueryComponent>(AITask->GetAIController()->BrainComponent) : nullptr;
}

//----------------------------------------------------------------------//
// DEPRECATED
//----------------------------------------------------------------------//
void USQNode::InitializeMemory(USelectionQueryComponent* OwnerComp, uint8* NodeMemory, ESQMemoryInit::Type InitType) const
{
	if (OwnerComp)
	{
		InitializeMemory(*OwnerComp, NodeMemory, InitType);
	}
}
void USQNode::CleanupMemory(USelectionQueryComponent* OwnerComp, uint8* NodeMemory, ESQMemoryClear::Type CleanupType) const
{
	if (OwnerComp)
	{
		CleanupMemory(*OwnerComp, NodeMemory, CleanupType);
	}
}
void USQNode::DescribeRuntimeValues(const USelectionQueryComponent* OwnerComp, uint8* NodeMemory, ESQDescriptionVerbosity::Type Verbosity, TArray<FString>& Values) const
{
	if (OwnerComp)
	{
		DescribeRuntimeValues(*OwnerComp, NodeMemory, Verbosity, Values);
	}
}
void USQNode::OnInstanceCreated(USelectionQueryComponent* OwnerComp)
{
	if (OwnerComp)
	{
		OnInstanceCreated(*OwnerComp);
	}
}
void USQNode::OnInstanceDestroyed(USelectionQueryComponent* OwnerComp)
{
	if (OwnerComp)
	{
		OnInstanceDestroyed(*OwnerComp);
	}
}
void USQNode::InitializeInSuSQree(USelectionQueryComponent* OwnerComp, uint8* NodeMemory, int32& NextInstancedIndex, ESQMemoryInit::Type InitType) const
{
	if (OwnerComp)
	{
		InitializeInSuSQree(*OwnerComp, NodeMemory, NextInstancedIndex, InitType);
	}
}
void USQNode::CleanupInSuSQree(USelectionQueryComponent* OwnerComp, uint8* NodeMemory, ESQMemoryClear::Type CleanupType) const
{
	if (OwnerComp)
	{
		CleanupInSuSQree(*OwnerComp, NodeMemory, CleanupType);
	}
}
USQNode* USQNode::GetNodeInstance(const USelectionQueryComponent* OwnerComp, uint8* NodeMemory) const
{
	if (OwnerComp)
	{
		return GetNodeInstance(*OwnerComp, NodeMemory);
	}
	return nullptr;
}
FString USQNode::GetRuntimeDescription(const USelectionQueryComponent* OwnerComp, uint8* NodeMemory, ESQDescriptionVerbosity::Type Verbosity) const
{
	if (OwnerComp)
	{
		return GetRuntimeDescription(*OwnerComp, NodeMemory, Verbosity);
	}
	return TEXT("");
}
