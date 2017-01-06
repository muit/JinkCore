// Copyright 2015-2017 Piperift. All Rights Reserved.

#include "SelectionQueryEditorPrivatePCH.h"

USQGraphNode_Root::USQGraphNode_Root(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    NodeName = "Root";
    bIsReadOnly = true;
}

void USQGraphNode_Root::AllocateDefaultPins()
{
    CreatePin(EGPD_Output, FSelectionQueryDataTypes::PinCategory_SingleComposite, TEXT(""), NULL, false, false, TEXT("In"));
}

FText USQGraphNode_Root::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
    return NSLOCTEXT("SelectionQueryEditor", "Root", "ROOT");
}
/*
FName USQGraphNode_Root::GetNameIcon() const
{
    return FName("BTEditor.Graph.BTNode.Root.Icon");
}*/
