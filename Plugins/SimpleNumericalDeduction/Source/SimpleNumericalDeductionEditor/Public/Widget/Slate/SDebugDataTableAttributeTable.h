//  Copyright (C) RenZhai.2021.All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "SimpleNumericalDeductionType.h"
#include "Widgets/SCompoundWidget.h"

class SSimepleNumbericalDeductionLog;
class SDebugDataTableAttributeTable :public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SDebugDataTableAttributeTable)
	{}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

public:
	FReply SaveAsText();
	FReply ClearLog();
	FReply Generate();

	TSharedPtr<SSimepleNumbericalDeductionLog> LogWidget;
};