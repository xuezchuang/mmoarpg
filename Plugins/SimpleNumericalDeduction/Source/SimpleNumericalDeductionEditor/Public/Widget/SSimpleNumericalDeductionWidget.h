// Copyright (C) RenZhai.2021.All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class SSimpleNumericalDeductionWidget :public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SSimpleNumericalDeductionWidget)
	{}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

public:
	FReply SaveAsDefault();
	FReply SaveAsCSV();
	FReply GenerateDeduction();
	FReply GenerateAttributeTable();
	FReply DebugDeductionData();
	bool IsDebugDeductionData()const;
	bool IsGenerateAttributeTable()const;
	bool IsGenerateDeduction()const;
	bool IsEnableToCSV()const;
private:
	void ClearDeductionValue();
	void GenerateDeductionWidget();

	void OpenMessageDialog(const FText& InText);
private:
	TSharedPtr<class SVerticalBox> VerticalList;
};
	 