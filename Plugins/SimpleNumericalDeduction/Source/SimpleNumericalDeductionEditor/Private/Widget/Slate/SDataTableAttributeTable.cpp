// Copyright (C) RenZhai.2021.All Rights Reserved.

#include "Widget/Slate/SDataTableAttributeTable.h"
#include "Widgets/Layout/SExpandableArea.h"
#include "Widget/Slate/SlateElement/STableBaseAttribute.h"
#include "Settings/SNDObjectSettings.h"
#include "SimpleNumericalDeduction.h"

#define LOCTEXT_NAMESPACE "SSDataTableAttributeTable"

void SSDataTableAttributeTable::Construct(const FArguments& InArgs, FDeduceAttributeDataTables& InDeduceAttributeDataTables)
{
	AttributeDataTablesPtr = &InDeduceAttributeDataTables;

	ChildSlot
	[
		SNew(SExpandableArea)
		.HeaderContent()
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			[
				SNew(SButton)
				.HAlign(HAlign_Center)
				.Text(this, &SSDataTableAttributeTable::GetTableName)
				.OnClicked(this, &SSDataTableAttributeTable::SpawnNewAttributeCurveTable)
				.ToolTipText(LOCTEXT("TableName_Tip", "Changes in the curve can be generated."))
			]

			+ SVerticalBox::Slot()
			[
				SNew(SHorizontalBox)
				
				+ SHorizontalBox::Slot()
				.HAlign(EHorizontalAlignment::HAlign_Left)
				.Padding(4.f, 2.f, 4.f, 2.f)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("SSDataTableAttributeTable_AttributeName", "Attribute Name"))
				]

				+ SHorizontalBox::Slot()
				.HAlign(EHorizontalAlignment::HAlign_Left)
				.Padding(4.f, 2.f, 4.f, 2.f)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("SSDataTableAttributeTable_Value", "Value"))
				]

				+ SHorizontalBox::Slot()
				.HAlign(EHorizontalAlignment::HAlign_Left)
				.Padding(4.f, 2.f, 4.f, 2.f)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("SSDataTableAttributeTable_AlgorithmCoefficient", "Coefficient"))
				]

				+ SHorizontalBox::Slot()
				.HAlign(EHorizontalAlignment::HAlign_Left)
				.Padding(4.f, 2.f, 4.f, 2.f)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("SSDataTableAttributeTable_CustomAlgorithm", "ActualCustomAlgorithm"))
				]
			]
		]
		.BodyContent()
		[
			SAssignNew(ListVerticalBox,SVerticalBox)
		]
	];

	if (USNDObjectSettings* InSND = const_cast<USNDObjectSettings*>(GetDefault<USNDObjectSettings>()))
	{
		for (int32 i = 0; i < AttributeDataTablesPtr->AttributeDatas.Num(); i++)
		{
			if (InSND->NumericalAlgorithmExecuteObject)
			{
				AttributeDataTablesPtr->AttributeDatas[i].BaseAlgorithm = InSND->NumericalAlgorithmExecuteObject;
			}
			else if (InSND->NumericalAlgorithmExecuteObjects.IsValidIndex(i))//按算法模板的位置进行匹配
			{
				if (InSND->NumericalAlgorithmExecuteObjects[i])
				{
					AttributeDataTablesPtr->AttributeDatas[i].BaseAlgorithm = InSND->NumericalAlgorithmExecuteObjects[i];
				}
			}

			if (InSND->Coefficient != 0.f)
			{
				AttributeDataTablesPtr->AttributeDatas[i].Coefficient = InSND->Coefficient;
			}
			else if (InSND->Coefficients.IsValidIndex(i))//按算法模板的位置进行匹配
			{
				if (InSND->Coefficients[i])
				{
					AttributeDataTablesPtr->AttributeDatas[i].Coefficient = InSND->Coefficients[i];
				}
			}

			//生成UI
			ListVerticalBox->AddSlot()
			.AutoHeight()
			[
				SNew(SSTableBaseAttribute, AttributeDataTablesPtr->AttributeDatas[i])
			];
		}
	}
}

FReply SSDataTableAttributeTable::SpawnNewAttributeCurveTable()
{
	if (AttributeDataTablesPtr)
	{
		FSimpleNumericalDeductionModule& SimpleNumericalDeductionModul = FModuleManager::LoadModuleChecked<FSimpleNumericalDeductionModule>("SimpleNumericalDeduction");
		SimpleNumericalDeductionModul.SpawnAttributeCurveTable(*AttributeDataTablesPtr);
	}

	return FReply::Handled();
}

FText SSDataTableAttributeTable::GetTableName() const
{
	if (AttributeDataTablesPtr)
	{
		return FText::FromName(AttributeDataTablesPtr->TableName);
	}

	return LOCTEXT("AttributeDataTables_TableName","NONE_Attribute");
}

#undef LOCTEXT_NAMESPACE 