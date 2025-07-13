// Copyright (C) RenZhai.2021.All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "SimpleNumericalDeductionStyle.h"

class FSimpleNumericalDeductionCommands : public TCommands<FSimpleNumericalDeductionCommands>
{
public:

	FSimpleNumericalDeductionCommands()
		: TCommands<FSimpleNumericalDeductionCommands>(TEXT("SimpleNumericalDeduction"), NSLOCTEXT("Contexts", "SimpleNumericalDeduction", "SimpleNumericalDeduction Plugin"), NAME_None, FSimpleNumericalDeductionStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > OpenPluginWindow;
};