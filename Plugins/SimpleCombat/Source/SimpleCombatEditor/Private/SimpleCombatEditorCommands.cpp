//Copyright (C) RenZhai.2022.All Rights Reserved.

#include "SimpleCombatEditorCommands.h"

#define LOCTEXT_NAMESPACE "FSimpleCombatEditorModule"

void FSimpleCombatEditorCommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "SimpleCombatEditor", "Execute SimpleCombatEditor action", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE
