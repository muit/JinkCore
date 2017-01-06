// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "SelectionQueryEditorPrivatePCH.h"
#include "SQItemNode.h"

USQGraphNode_Item::USQGraphNode_Item(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void USQGraphNode_Item::AllocateDefaultPins()
{
    CreatePin(EGPD_Input, TEXT("Transition"), TEXT(""), NULL, false, false, TEXT("In"));
}

FText USQGraphNode_Item::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
    const USQNode* MyNode = Cast<USQNode>(NodeInstance);
    if (MyNode != NULL)
    {
        return FText::FromString(MyNode->GetNodeName());
    }
    else if (!ClassData.GetClassName().IsEmpty())
    {
        FString StoredClassName = ClassData.GetClassName();
        StoredClassName.RemoveFromEnd(TEXT("_C"));

        return FText::Format(NSLOCTEXT("SQGraph", "NodeClassError", "Class {0} not found, make sure it's saved!"), FText::FromString(StoredClassName));
    }

    return Super::GetNodeTitle(TitleType);
}

void USQGraphNode_Item::GetContextMenuActions(const FGraphNodeContextMenuBuilder& Context) const
{
    //AddContextMenuActionsDecorators(Context);
    //AddContextMenuActionsServices(Context);
}
