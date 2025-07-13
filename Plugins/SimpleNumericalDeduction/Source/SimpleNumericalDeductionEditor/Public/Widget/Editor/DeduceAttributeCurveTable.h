// Copyright (C) RenZhai.2021.All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Widget/Editor/Core/DeduceEditor.h"
#include "SimpleNumericalDeductionType.h"

class SCurveEditorPanel;
class FCurveEditor;
class FSDeduceAttributeCurveTable :public FDeduceEditor
{
public:
	FSDeduceAttributeCurveTable();
	virtual ~FSDeduceAttributeCurveTable(){ }
	virtual void InitLayout();

public:
	void Construct(FDeduceAttributeDataTables& InDeduceAttributeDataTables);
	void Construct(FDeduceAttributeData& InDeduceAttributeDataTable);

	TSharedRef<SDockTab> SpawnTab_CurveAsset(const FSpawnTabArgs& Args);

private:
	TSharedPtr<FCurveEditor> CurveEditor;
	TSharedPtr<SCurveEditorPanel> CurveEditorPanel;

	FDeduceAttributeDataTables* DeduceAttributeDataTables;
	FDeduceAttributeData* DeduceAttributeDataTable;
};