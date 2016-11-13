// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "JinkCorePrivatePCH.h"
#include "GameplayTasksComponent.h"
#include "SelectionQuery/SQTaskNode.h"

USQTaskNode::USQTaskNode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bNotifyTick = false;
	bNotifyTaskFinished = false;
	bIgnoreRestartSelf = false;
}

ESQNodeResult::Type USQTaskNode::ExecuteTask(USelectionQueryComponent& OwnerComp, uint8* NodeMemory)
{
	return ESQNodeResult::Succeeded;
}

ESQNodeResult::Type USQTaskNode::AbortTask(USelectionQueryComponent& OwnerComp, uint8* NodeMemory)
{
	return ESQNodeResult::Aborted;
}

ESQNodeResult::Type USQTaskNode::WrappedExecuteTask(USelectionQueryComponent& OwnerComp, uint8* NodeMemory) const
{
	const USQNode* NodeOb = bCreateNodeInstance ? GetNodeInstance(OwnerComp, NodeMemory) : this;
	return NodeOb ? ((USQTaskNode*)NodeOb)->ExecuteTask(OwnerComp, NodeMemory) : ESQNodeResult::Failed;
}

ESQNodeResult::Type USQTaskNode::WrappedAbortTask(USelectionQueryComponent& OwnerComp, uint8* NodeMemory) const
{
	USQNode* NodeOb = const_cast<USQNode*>(bCreateNodeInstance ? GetNodeInstance(OwnerComp, NodeMemory) : this);
	USQTaskNode* TaskNodeOb = static_cast<USQTaskNode*>(NodeOb);
	ESQNodeResult::Type Result = TaskNodeOb ? TaskNodeOb->AbortTask(OwnerComp, NodeMemory) : ESQNodeResult::Aborted;

	return Result;
}

void USQTaskNode::WrappedTickTask(USelectionQueryComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) const
{
	if (bNotifyTick)
	{
		const USQNode* NodeOb = bCreateNodeInstance ? GetNodeInstance(OwnerComp, NodeMemory) : this;
		if (NodeOb)
		{
			((USQTaskNode*)NodeOb)->TickTask(OwnerComp, NodeMemory, DeltaSeconds);
		}
	}
}

void USQTaskNode::WrappedOnTaskFinished(USelectionQueryComponent& OwnerComp, uint8* NodeMemory, ESQNodeResult::Type TaskResult) const
{
	USQNode* NodeOb = const_cast<USQNode*>(bCreateNodeInstance ? GetNodeInstance(OwnerComp, NodeMemory) : this);

	if (NodeOb)
	{
		USQTaskNode* TaskNodeOb = static_cast<USQTaskNode*>(NodeOb);
		if (TaskNodeOb->bNotifyTaskFinished)
		{
			TaskNodeOb->OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
		}

		if (TaskNodeOb->bOwnsGameplayTasks && OwnerComp.GetAIOwner())
		{
			UGameplayTasksComponent* GTComp = OwnerComp.GetAIOwner()->GetGameplayTasksComponent();
			if (GTComp)
			{
				GTComp->EndAllResourceConsumingTasksOwnedBy(*TaskNodeOb);
			}
		}
	}
}

void USQTaskNode::ReceivedMessage(UBrainComponent* BrainComp, const FAIMessage& Message)
{
	USelectionQueryComponent* OwnerComp = static_cast<USelectionQueryComponent*>(BrainComp);
	check(OwnerComp);
	
	const uint16 InstanceIdx = OwnerComp->FindInstanceContainingNode(this);
	if (OwnerComp->InstanceStack.IsValidIndex(InstanceIdx))
	{
		uint8* NodeMemory = GetNodeMemory<uint8>(OwnerComp->InstanceStack[InstanceIdx]);
		OnMessage(*OwnerComp, NodeMemory, Message.MessageName, Message.RequestID, Message.Status == FAIMessage::Success);
	}
	else
	{
		UE_VLOG(OwnerComp->GetOwner(), LogSelectionQuery, Warning, TEXT("USQTaskNode::ReceivedMessage called while %s node no longer in active SQ")
			, *GetNodeName());
	}
}

void USQTaskNode::TickTask(USelectionQueryComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	// empty in base class
}

void USQTaskNode::OnTaskFinished(USelectionQueryComponent& OwnerComp, uint8* NodeMemory, ESQNodeResult::Type TaskResult)
{
	// empty in base class
}

void USQTaskNode::OnMessage(USelectionQueryComponent& OwnerComp, uint8* NodeMemory, FName Message, int32 RequestID, bool bSuccess)
{
	const ESQTaskStatus::Type Status = OwnerComp.GetTaskStatus(this);
	if (Status == ESQTaskStatus::Active)
	{
		FinishLatentTask(OwnerComp, bSuccess ? ESQNodeResult::Succeeded : ESQNodeResult::Failed);
	}
	else if (Status == ESQTaskStatus::Aborting)
	{
		FinishLatentAbort(OwnerComp);
	}
}

void USQTaskNode::FinishLatentTask(USelectionQueryComponent& OwnerComp, ESQNodeResult::Type TaskResult) const
{
	// OnTaskFinished must receive valid template node
	USQTaskNode* TemplateNode = (USQTaskNode*)OwnerComp.FindTemplateNode(this);
	OwnerComp.OnTaskFinished(TemplateNode, TaskResult);
}

void USQTaskNode::FinishLatentAbort(USelectionQueryComponent& OwnerComp) const
{
	// OnTaskFinished must receive valid template node
	USQTaskNode* TemplateNode = (USQTaskNode*)OwnerComp.FindTemplateNode(this);
	OwnerComp.OnTaskFinished(TemplateNode, ESQNodeResult::Aborted);
}

void USQTaskNode::WaitForMessage(USelectionQueryComponent& OwnerComp, FName MessageType) const
{
	// messages delegates should be called on node instances (if they exists)
	OwnerComp.RegisterMessageObserver(this, MessageType);
}

void USQTaskNode::WaitForMessage(USelectionQueryComponent& OwnerComp, FName MessageType, int32 RequestID) const
{
	// messages delegates should be called on node instances (if they exists)
	OwnerComp.RegisterMessageObserver(this, MessageType, RequestID);
}
	
void USQTaskNode::StopWaitingForMessages(USelectionQueryComponent& OwnerComp) const
{
	// messages delegates should be called on node instances (if they exists)
	OwnerComp.UnregisterMessageObserversFrom(this);
}

#if WITH_EDITOR

FName USQTaskNode::GetNodeIconName() const
{
	return FName("SQEditor.Graph.SQNode.Task.Icon");
}

#endif	// WITH_EDITOR

void USQTaskNode::OnGameplayTaskDeactivated(UGameplayTask& Task)
{
	ensure(Task.GetTaskOwner() == this);

	USelectionQueryComponent* SQComp = GetSQComponentForTask(Task);
	if (SQComp)
	{
		// this is a super-default behavior. Specific task will surely like to 
		// handle this themselves, finishing with specific result
		const ESQTaskStatus::Type Status = SQComp->GetTaskStatus(this);
		FinishLatentTask(*SQComp, Status == ESQTaskStatus::Aborting ? ESQNodeResult::Aborted : ESQNodeResult::Succeeded);
	}
}

//----------------------------------------------------------------------//
// DEPRECATED
//----------------------------------------------------------------------//
ESQNodeResult::Type USQTaskNode::WrappedExecuteTask(USelectionQueryComponent* OwnerComp, uint8* NodeMemory) const
{
	return OwnerComp ? WrappedExecuteTask(*OwnerComp, NodeMemory) : ESQNodeResult::Failed;
}
ESQNodeResult::Type USQTaskNode::WrappedAbortTask(USelectionQueryComponent* OwnerComp, uint8* NodeMemory) const
{
	return OwnerComp ? WrappedAbortTask(*OwnerComp, NodeMemory) : ESQNodeResult::Failed;
}
void USQTaskNode::WrappedTickTask(USelectionQueryComponent* OwnerComp, uint8* NodeMemory, float DeltaSeconds) const
{
	if (OwnerComp)
	{
		WrappedTickTask(*OwnerComp, NodeMemory, DeltaSeconds);
	}
}
void USQTaskNode::WrappedOnTaskFinished(USelectionQueryComponent* OwnerComp, uint8* NodeMemory, ESQNodeResult::Type TaskResult) const
{
	if (OwnerComp)
	{
		WrappedOnTaskFinished(*OwnerComp, NodeMemory, TaskResult);
	}
}
void USQTaskNode::FinishLatentTask(USelectionQueryComponent* OwnerComp, ESQNodeResult::Type TaskResult) const
{
	if (OwnerComp)
	{
		FinishLatentTask(*OwnerComp, TaskResult);
	}
}
void USQTaskNode::FinishLatentAbort(USelectionQueryComponent* OwnerComp) const
{
	if (OwnerComp)
	{
		FinishLatentAbort(*OwnerComp);
	}
}
ESQNodeResult::Type USQTaskNode::ExecuteTask(USelectionQueryComponent* OwnerComp, uint8* NodeMemory)
{
	if (OwnerComp)
	{
		return ExecuteTask(*OwnerComp, NodeMemory);
	}
	return ESQNodeResult::Failed;
}
ESQNodeResult::Type USQTaskNode::AbortTask(USelectionQueryComponent* OwnerComp, uint8* NodeMemory)
{
	if (OwnerComp)
	{
		return AbortTask(*OwnerComp, NodeMemory);
	}
	return ESQNodeResult::Failed;
}
void USQTaskNode::TickTask(USelectionQueryComponent* OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	if (OwnerComp)
	{
		TickTask(*OwnerComp, NodeMemory, DeltaSeconds);
	}
}
void USQTaskNode::OnMessage(USelectionQueryComponent* OwnerComp, uint8* NodeMemory, FName Message, int32 RequestID, bool bSuccess)
{
	if (OwnerComp)
	{
		OnMessage(*OwnerComp, NodeMemory, Message, RequestID, bSuccess);
	}
}
void USQTaskNode::OnTaskFinished(USelectionQueryComponent* OwnerComp, uint8* NodeMemory, ESQNodeResult::Type TaskResult)
{
	if (OwnerComp)
	{
		OnTaskFinished(*OwnerComp, NodeMemory, TaskResult);
	}
}
void USQTaskNode::WaitForMessage(USelectionQueryComponent* OwnerComp, FName MessageType) const
{
	if (OwnerComp)
	{
		WaitForMessage(*OwnerComp, MessageType);
	}
}
void USQTaskNode::WaitForMessage(USelectionQueryComponent* OwnerComp, FName MessageType, int32 RequestID) const
{
	if (OwnerComp)
	{
		WaitForMessage(*OwnerComp, MessageType, RequestID);
	}
}