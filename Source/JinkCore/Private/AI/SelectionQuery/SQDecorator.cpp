// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "JinkCorePrivatePCH.h"
#include "SelectionQuery/SQCompositeNode.h"
#include "SelectionQuery/SQDecorator.h"

USQDecorator::USQDecorator(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	FlowAbortMode = ESQFlowAbortMode::None;
	bAllowAbortNone = true;
	bAllowAbortLowerPri = true;
	bAllowAbortChildNodes = true;
	bNotifyActivation = false;
	bNotifyDeactivation = false;
	bNotifyProcessed = false;

	bShowInverseConditionDesc = true;
	bInverseCondition = false;
}

bool USQDecorator::CalculateRawConditionValue(USelectionQueryComponent& OwnerComp, uint8* NodeMemory) const
{
	return true;
}

void USQDecorator::SetIsInversed(bool bShouldBeInversed)
{
	bInverseCondition = bShouldBeInversed;
}

void USQDecorator::OnNodeActivation(FSelectionQuerySearchData& SearchData)
{
}

void USQDecorator::OnNodeDeactivation(FSelectionQuerySearchData& SearchData, ESQNodeResult::Type NodeResult)
{
}

void USQDecorator::OnNodeProcessed(FSelectionQuerySearchData& SearchData, ESQNodeResult::Type& NodeResult)
{
}

bool USQDecorator::WrappedCanExecute(USelectionQueryComponent& OwnerComp, uint8* NodeMemory) const
{
	const USQDecorator* NodeOb = bCreateNodeInstance ? (const USQDecorator*)GetNodeInstance(OwnerComp, NodeMemory) : this;
	return NodeOb ? (IsInversed() != NodeOb->CalculateRawConditionValue(OwnerComp, NodeMemory)) : false;
}

void USQDecorator::WrappedOnNodeActivation(FSelectionQuerySearchData& SearchData) const
{
	if (bNotifyActivation)
	{
		const USQNode* NodeOb = bCreateNodeInstance ? GetNodeInstance(SearchData) : this;
		if (NodeOb)
		{
			((USQDecorator*)NodeOb)->OnNodeActivation(SearchData);
		}		
	}
};

void USQDecorator::WrappedOnNodeDeactivation(FSelectionQuerySearchData& SearchData, ESQNodeResult::Type NodeResult) const
{
	if (bNotifyDeactivation)
	{
		const USQNode* NodeOb = bCreateNodeInstance ? GetNodeInstance(SearchData) : this;
		if (NodeOb)
		{
			((USQDecorator*)NodeOb)->OnNodeDeactivation(SearchData, NodeResult);
		}		
	}
}

void USQDecorator::WrappedOnNodeProcessed(FSelectionQuerySearchData& SearchData, ESQNodeResult::Type& NodeResult) const
{
	if (bNotifyProcessed)
	{
		const USQNode* NodeOb = bCreateNodeInstance ? GetNodeInstance(SearchData) : this;
		if (NodeOb)
		{
			((USQDecorator*)NodeOb)->OnNodeProcessed(SearchData, NodeResult);
		}		
	}
}

FString USQDecorator::GetStaticDescription() const
{
	FString FlowAbortDesc;
	if (FlowAbortMode != ESQFlowAbortMode::None)
	{
		FlowAbortDesc = FString::Printf(TEXT("aborts %s"), *USelectionQueryTypes::DescribeFlowAbortMode(FlowAbortMode).ToLower());
	}

	FString InversedDesc;
	if (bShowInverseConditionDesc && IsInversed())
	{
		InversedDesc = TEXT("inversed");
	}

	FString AdditionalDesc;
	if (FlowAbortDesc.Len() || InversedDesc.Len())
	{
		AdditionalDesc = FString::Printf(TEXT("( %s%s%s )\n"), *FlowAbortDesc, 
			(FlowAbortDesc.Len() > 0) && (InversedDesc.Len() > 0) ? TEXT(", ") : TEXT(""),
			*InversedDesc);
	}

	return FString::Printf(TEXT("%s%s"), *AdditionalDesc, *USelectionQueryTypes::GetShortTypeName(this));
}

bool USQDecorator::IsFlowAbortModeValid() const
{
#if WITH_EDITOR
	if (GetParentNode() == NULL ||
		(GetParentNode()->CanAbortLowerPriority() == false && GetParentNode()->CanAbortSelf() == false))
	{
		return (FlowAbortMode == ESQFlowAbortMode::None);
	}

	if (GetParentNode()->CanAbortLowerPriority() == false)
	{
		return (FlowAbortMode == ESQFlowAbortMode::None || FlowAbortMode == ESQFlowAbortMode::Self);
	}

	if (GetParentNode()->CanAbortSelf() == false)
	{
		return (FlowAbortMode == ESQFlowAbortMode::None || FlowAbortMode == ESQFlowAbortMode::LowerPriority);
	}
#endif

	return true;
}

void USQDecorator::UpdateFlowAbortMode()
{
#if WITH_EDITOR
	if (GetParentNode() == NULL)
	{
		FlowAbortMode = ESQFlowAbortMode::None;
		return;
	}

	if (GetParentNode()->CanAbortLowerPriority() == false)
	{
		if (FlowAbortMode == ESQFlowAbortMode::Both)
		{
			FlowAbortMode = GetParentNode()->CanAbortSelf() ? ESQFlowAbortMode::Self : ESQFlowAbortMode::None;
		}
		else if (FlowAbortMode == ESQFlowAbortMode::LowerPriority)
		{
			FlowAbortMode = ESQFlowAbortMode::None;
		}
	}

	if (GetParentNode()->CanAbortSelf() == false)
	{
		if (FlowAbortMode == ESQFlowAbortMode::Both)
		{
			FlowAbortMode = GetParentNode()->CanAbortLowerPriority() ? ESQFlowAbortMode::LowerPriority : ESQFlowAbortMode::None;
		}
		else if (FlowAbortMode == ESQFlowAbortMode::Self)
		{
			FlowAbortMode = ESQFlowAbortMode::None;
		}
	}
#endif
}

//----------------------------------------------------------------------//
// DEPRECATED
//----------------------------------------------------------------------//
bool USQDecorator::CalculateRawConditionValue(USelectionQueryComponent* OwnerComp, uint8* NodeMemory) const
{
	return OwnerComp ? CalculateRawConditionValue(*OwnerComp, NodeMemory) : false;
}

void USQDecorator::InitializeDecorator(uint8 InChildIndex)
{
	InitializeParentLink(InChildIndex);
}
