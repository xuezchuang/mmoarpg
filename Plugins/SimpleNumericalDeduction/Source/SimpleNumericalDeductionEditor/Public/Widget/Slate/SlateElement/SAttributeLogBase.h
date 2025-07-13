// Copyright (C) RenZhai.2021.All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "SimpleNumericalDeductionType.h"
#include "Widgets/SCompoundWidget.h"

class SAttributeLogBase :public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SAttributeLogBase)
	{}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, const FSimplePreDebugPrintf& InMyLog);
};
