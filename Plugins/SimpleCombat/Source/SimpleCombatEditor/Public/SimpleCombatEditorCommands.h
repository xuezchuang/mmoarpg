//Copyright (C) RenZhai.2022.All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "SimpleCombatEditorStyle.h"

class FSimpleCombatEditorCommands : public TCommands<FSimpleCombatEditorCommands>
{
public:

	FSimpleCombatEditorCommands()
		: TCommands<FSimpleCombatEditorCommands>(TEXT("SimpleCombatEditor"), NSLOCTEXT("Contexts", "SimpleCombatEditor", "SimpleCombatEditor Plugin"), NAME_None, FSimpleCombatEditorStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > PluginAction;
};
