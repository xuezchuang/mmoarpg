// Copyright (C) RenZhai.2021.All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "SimpleNumericalDeductionType.h"
#include "Widgets/SCompoundWidget.h"

class SVerticalBox;
class SSDataTableAttributeTable :public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SSDataTableAttributeTable)
	{}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs,FDeduceAttributeDataTables &InDeduceAttributeDataTables);

public:
	FReply SpawnNewAttributeCurveTable();
	FText GetTableName() const;

private:
	TSharedPtr<SVerticalBox> ListVerticalBox;
	FDeduceAttributeDataTables* AttributeDataTablesPtr;
};
