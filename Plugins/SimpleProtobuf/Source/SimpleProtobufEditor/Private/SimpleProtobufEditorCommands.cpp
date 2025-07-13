// Copyright (C) RenZhai.2021.All Rights Reserved.

#include "SimpleProtobufEditorCommands.h"

#define LOCTEXT_NAMESPACE "FSimpleProtobufEditorModule"

void FSimpleProtobufEditorCommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "CompilingProtobuf", "Compile protobuf to c++ file.", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE
