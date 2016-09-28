// Copyright 2015-2016 Piperift. All Rights Reserved.

#include "JinkEditor.h"
 

DEFINE_LOG_CATEGORY(JinkEditor)
 
#define LOCTEXT_NAMESPACE "JinkEditor"
 
void FJinkEditorModule::StartupModule()
{
	UE_LOG(JinkEditor, Warning, TEXT("JinkEditor: Log Started"));
}
 
void FJinkEditorModule::ShutdownModule()
{
	UE_LOG(JinkEditor, Warning, TEXT("JinkEditor: Log Ended"));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_GAME_MODULE(FJinkEditorModule, JinkEditor);