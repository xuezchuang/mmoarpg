// Copyright (C) RenZhai.2021.All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "SimpleNumericalDeductionType.h"
#include "Widgets/SCompoundWidget.h"

class SEditableTextBox;
class SSTableBaseAttribute :public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SSTableBaseAttribute)
	{}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, FDeduceAttributeData& InDeduceAttributeDataTables);

public:
	FReply SpawnSingleAttributeCurveTable();
	void OnValueChanged(float InValue);
	void OnValueCommied(const FText& InText, ETextCommit::Type InType);
private:
	FDeduceAttributeData* AttributeDataTablesPtr;
	TSharedPtr<SEditableTextBox> EditableTextBox;
};
