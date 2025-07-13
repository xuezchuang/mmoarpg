// Copyright (C) RenZhai.2021.All Rights Reserved.

#include "Widget/Slate/SlateElement/STableBaseAttribute.h"
#include "Widgets/Input/SSpinBox.h"
#include "UObject/NumericalAlgorithmExecuteObject.h"
#include "SimpleNumericalDeduction.h"

void SSTableBaseAttribute::Construct(const FArguments& InArgs, FDeduceAttributeData& InDeduceAttributeDataTables)
{
	AttributeDataTablesPtr = &InDeduceAttributeDataTables;

	ChildSlot
	[
		SNew(SHorizontalBox)

		+ SHorizontalBox::Slot()
		.HAlign(EHorizontalAlignment::HAlign_Fill)
		.Padding(4.f, 2.f, 4.f, 2.f)
		[
			SNew(SButton)
			.Text(FText::FromName(InDeduceAttributeDataTables.Key))
			.HAlign(HAlign_Fill)
			.OnClicked(this, &SSTableBaseAttribute::SpawnSingleAttributeCurveTable)
		]

		+ SHorizontalBox::Slot()
		.HAlign(EHorizontalAlignment::HAlign_Left)
		.Padding(4.f, 2.f, 4.f, 2.f)
		[
			SNew(STextBlock)
			.Text(FText::FromString(InDeduceAttributeDataTables.Value))
		]

		+ SHorizontalBox::Slot()
		.HAlign(EHorizontalAlignment::HAlign_Left)
		.Padding(4.f, 2.f, 4.f, 2.f)
		[
			SNew(SSpinBox<float>)
			.Value(InDeduceAttributeDataTables.Coefficient)
			.OnValueChanged(this,&SSTableBaseAttribute::OnValueChanged)
		]

		+ SHorizontalBox::Slot()
		.HAlign(EHorizontalAlignment::HAlign_Fill)
		.Padding(4.f, 2.f, 4.f, 2.f)
		[
			SAssignNew(EditableTextBox,SEditableTextBox)
			.OnTextCommitted(this,&SSTableBaseAttribute::OnValueCommied)
		]

	/*	+ SHorizontalBox::Slot()
		.HAlign(EHorizontalAlignment::HAlign_Fill)
		.Padding(4.f, 2.f, 4.f, 2.f)
		[
			SAssignNew(EditableTextBox, SEditableTextBox)
			.OnTextCommitted(this, &SSTableBaseAttribute::OnValueCommied)
		]*/
	];

	//初始化算法字体引用对象
	if (EditableTextBox && AttributeDataTablesPtr && AttributeDataTablesPtr->BaseAlgorithm)
	{
		FString FullName = AttributeDataTablesPtr->BaseAlgorithm->GetFullName();
		FullName.RemoveFromStart(TEXT("Class "));
		FullName.RemoveFromStart(TEXT("BlueprintGeneratedClass "));

		EditableTextBox->SetText(FText::FromString(FullName));
	}
}

void SSTableBaseAttribute::OnValueChanged(float InValue)
{
	if (AttributeDataTablesPtr)
	{
		AttributeDataTablesPtr->Coefficient = InValue;
	}
}

void SSTableBaseAttribute::OnValueCommied(const FText& InText, ETextCommit::Type InType)
{
	if (AttributeDataTablesPtr)
	{
		if (UClass *InClass = StaticLoadClass(UNumericalAlgorithmExecuteObject::StaticClass(),NULL,*InText.ToString()))
		{
			AttributeDataTablesPtr->BaseAlgorithm = InClass;
		}
		else
		{
			AttributeDataTablesPtr->BaseAlgorithm = UNumericalAlgorithmExecuteObject::StaticClass();
		}
	}
}

FReply SSTableBaseAttribute::SpawnSingleAttributeCurveTable()
{
	if (AttributeDataTablesPtr)
	{
		FSimpleNumericalDeductionModule& SimpleNumericalDeductionModul = FModuleManager::LoadModuleChecked<FSimpleNumericalDeductionModule>("SimpleNumericalDeduction");
		SimpleNumericalDeductionModul.SpawnAttributeCurveTable(*AttributeDataTablesPtr);
	}

	return FReply::Handled();
}
