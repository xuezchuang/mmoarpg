// Copyright (C) RenZhai.2021.All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "SimpleProtobufEditorStyle.h"

class FSimpleProtobufEditorCommands : public TCommands<FSimpleProtobufEditorCommands>
{
public:

	FSimpleProtobufEditorCommands()
		: TCommands<FSimpleProtobufEditorCommands>(TEXT("SimpleProtobufEditor"), NSLOCTEXT("Contexts", "SimpleProtobufEditor", "SimpleProtobufEditor Plugin"), NAME_None, FSimpleProtobufEditorStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > PluginAction;
};
