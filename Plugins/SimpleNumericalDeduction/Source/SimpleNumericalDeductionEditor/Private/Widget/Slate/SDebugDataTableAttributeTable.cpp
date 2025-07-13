// Copyright (C) RenZhai.2021.All Rights Reserved.
#include "Widget/Slate/SDebugDataTableAttributeTable.h"
#include "PropertyEditorModule.h"
#include "Settings/SNDNumericalBalanceDebugSettings.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SBox.h"
#include "Widget/Slate/SSimepleNumbericalDeductionLog.h"

#define LOCTEXT_NAMESPACE "SDebugDataTableAttributeTable"

void SDebugDataTableAttributeTable::Construct(const FArguments& InArgs)
{
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked< FPropertyEditorModule >("PropertyEditor");

	FDetailsViewArgs DetailsViewArgs;
	{
		DetailsViewArgs.bAllowMultipleTopLevelObjects = true;
		DetailsViewArgs.bShowPropertyMatrixButton = false;
	}

	TSharedPtr<class IDetailsView> ConfigPanel = PropertyModule.CreateDetailView(DetailsViewArgs);
	TArray<UObject*> SourceObjects;
	{
		SourceObjects.Add(const_cast<USNDNumericalBalanceDebugSettings*>(GetDefault<USNDNumericalBalanceDebugSettings>()));
	}

	ConfigPanel->SetObjects(SourceObjects);

	ChildSlot
	[
		SNew(SVerticalBox)

		+ SVerticalBox::Slot()
		.HAlign(EHorizontalAlignment::HAlign_Right)
		.AutoHeight()
		.Padding(4.f, 2.f, 4.f, 2.f)
		[
			SNew(SButton)
			.Text(LOCTEXT("SND_Generate", "Generate"))
			.HAlign(HAlign_Center)
			.OnClicked(this, &SDebugDataTableAttributeTable::Generate)
			.ToolTipText(LOCTEXT("SND_GenerateTip", "Store configuration locally."))
		]
		//反射界面
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			ConfigPanel.ToSharedRef()
		]

		//日志
		+ SVerticalBox::Slot()
		.FillHeight(1.f)
		.Padding(4.f, 2.f, 4.f, 2.f)
		[
			SNew(SScrollBox)
			.Orientation(Orient_Vertical)

			+ SScrollBox::Slot()
			[
				SAssignNew(LogWidget, SSimepleNumbericalDeductionLog)
			]
		]

		//底部的button
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
			.HAlign(EHorizontalAlignment::HAlign_Right)
			.Padding(4.f, 2.f, 4.f, 2.f)
			[
				SNew(SButton)
				.Text(LOCTEXT("SND_SaveAsText", "Save as Text"))
				.HAlign(HAlign_Center)
				.OnClicked(this, &SDebugDataTableAttributeTable::SaveAsText)
				.ToolTipText(LOCTEXT("SND_SaveAsTextTip", "Store configuration locally."))
			]

			+ SHorizontalBox::Slot()
			.HAlign(EHorizontalAlignment::HAlign_Right)
			.Padding(4.f, 2.f, 4.f, 2.f)
			.AutoWidth()
			[
				SNew(SButton)
				.Text(LOCTEXT("SND_ClearLog_csv", "Clear Log"))
				.HAlign(HAlign_Center)
				.OnClicked(this, &SDebugDataTableAttributeTable::ClearLog)
				.ToolTipText(LOCTEXT("SND_ClearLog_csvTip", "This function is mainly used for calling gameplay system after exporting deduction results."))
			]
		]	
	];
}

FReply SDebugDataTableAttributeTable::Generate()
{
	if (LogWidget.IsValid())
	{
		LogWidget->Generate();
	}

	return FReply::Handled();
}

FReply SDebugDataTableAttributeTable::SaveAsText()
{
	if (LogWidget.IsValid())
	{
		LogWidget->SaveAsText();
	}

	return FReply::Handled();
}

FReply SDebugDataTableAttributeTable::ClearLog()
{
	if (LogWidget.IsValid())
	{
		LogWidget->ClearLog();
	}

	return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE