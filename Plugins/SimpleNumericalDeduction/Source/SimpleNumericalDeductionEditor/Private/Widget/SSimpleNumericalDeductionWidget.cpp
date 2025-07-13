// Copyright (C) RenZhai.2021.All Rights Reserved.

#include "Widget/SSimpleNumericalDeductionWidget.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SBox.h"
#include "PropertyEditorModule.h"
#include "Settings/SNDObjectSettings.h"
#include "Widget/Slate/SDataTableAttributeTable.h"
#include "UObject/NumericalAlgorithmExecuteObject.h"
#include "Misc/FileHelper.h"
#include "SimpleNumericalDeduction.h"

#define LOCTEXT_NAMESPACE "SimpleNumericalDeductionWidget"

void SSimpleNumericalDeductionWidget::Construct(const FArguments& InArgs)
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
		SourceObjects.Add(const_cast<USNDObjectSettings*>(GetDefault<USNDObjectSettings>()));
	}

	ConfigPanel->SetObjects(SourceObjects);
	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
			.HAlign(EHorizontalAlignment::HAlign_Right)
			.Padding(4.f, 2.f, 4.f, 2.f)
			[
				SNew(SButton)
				.Text(LOCTEXT("SND_Save_as_Default", "Save as Default"))
				.HAlign(HAlign_Center)
				.OnClicked(this, &SSimpleNumericalDeductionWidget::SaveAsDefault)
				.ToolTipText(LOCTEXT("SND_Save_as_DefaultTip", "Store configuration locally."))
			]

			+ SHorizontalBox::Slot()
			.HAlign(EHorizontalAlignment::HAlign_Right)
			.Padding(4.f, 2.f, 4.f, 2.f)
			.AutoWidth()
			[
				SNew(SButton)
				.Text(LOCTEXT("SND_Save_as_csv", "Save as csv"))
				.HAlign(HAlign_Center)
				.IsEnabled(this, &SSimpleNumericalDeductionWidget::IsEnableToCSV)
				.OnClicked(this, &SSimpleNumericalDeductionWidget::SaveAsCSV)
				.ToolTipText(LOCTEXT("SND_Save_as_CSVTip", "This function is mainly used for calling gameplay system after exporting deduction results."))
			]
			+ SHorizontalBox::Slot()
			.HAlign(EHorizontalAlignment::HAlign_Right)
			.Padding(4.f, 2.f, 4.f, 2.f)
			.AutoWidth()
			[
				SNew(SButton)
				.Text(LOCTEXT("Generate_Attribute_Table", "Generate Attribute Table"))
				.HAlign(HAlign_Center)
				.IsEnabled(this, &SSimpleNumericalDeductionWidget::IsGenerateAttributeTable)
				.OnClicked(this, &SSimpleNumericalDeductionWidget::GenerateAttributeTable)
				.ToolTipText(LOCTEXT("Generate_Attribute_TableTip", "Generate Attribute data according to the table."))
			]

			+ SHorizontalBox::Slot()
			.HAlign(EHorizontalAlignment::HAlign_Right)
			.Padding(4.f, 2.f, 4.f, 2.f)
			.AutoWidth()
			[
				SNew(SButton)
				.Text(LOCTEXT("Generate_Deduction", "Generate Deduction"))
				.HAlign(HAlign_Center)
				.IsEnabled(this, &SSimpleNumericalDeductionWidget::IsGenerateDeduction)
				.OnClicked(this, &SSimpleNumericalDeductionWidget::GenerateDeduction)
				.ToolTipText(LOCTEXT("Generate_DeductionTip", "Generate deduction data according to the table."))
			]

			+ SHorizontalBox::Slot()
			.HAlign(EHorizontalAlignment::HAlign_Right)
			.Padding(4.f, 2.f, 4.f, 2.f)
			.AutoWidth()
			[
				SNew(SButton)
				.Text(LOCTEXT("Debug_Deduction_Deduction", "Debug Deduction Data"))
				.HAlign(HAlign_Center)
				.IsEnabled(this, &SSimpleNumericalDeductionWidget::IsDebugDeductionData)
				.OnClicked(this, &SSimpleNumericalDeductionWidget::DebugDeductionData)
				.ToolTipText(LOCTEXT("Debug_Deduction_DeductionTip", "Debug deduction deduction data according to the table."))
			]
		]

		+ SVerticalBox::Slot()
		.FillHeight(1.f)
		[
			SNew(SScrollBox)
			.Orientation(Orient_Vertical)

			+ SScrollBox::Slot()
			[
				ConfigPanel.ToSharedRef()
			]

			+ SScrollBox::Slot()
			[
				SAssignNew(VerticalList, SVerticalBox)
			]
		]
	
	];

	//读取我们的配置
	if(USNDObjectSettings * SND = const_cast<USNDObjectSettings*>(GetDefault<USNDObjectSettings>()))
	{
		SND->LoadObjectConfig();
	}

	//生成表
	GenerateDeductionWidget();
}

void SSimpleNumericalDeductionWidget::OpenMessageDialog(const FText& InText)
{
	FMessageDialog::Open(EAppMsgType::YesNo, InText);
}

void SSimpleNumericalDeductionWidget::GenerateDeductionWidget()
{
	if (VerticalList)
	{
		if (USNDObjectSettings* SND = const_cast<USNDObjectSettings*>(GetDefault<USNDObjectSettings>()))
		{
			//清除所以子项
			VerticalList->ClearChildren();

			for (auto& Tmp : SND->AttributeDatas)
			{
				VerticalList->AddSlot()
				.AutoHeight()
				[
					SNew(SSDataTableAttributeTable, Tmp)
				];
			}		
		}
	}
}

FReply SSimpleNumericalDeductionWidget::GenerateAttributeTable()
{
	if (USNDObjectSettings* SND = const_cast<USNDObjectSettings*>(GetDefault<USNDObjectSettings>()))
	{
		if (SND->AnalysisBaseTable())
		{
			GenerateDeductionWidget();
		}
	}

	OpenMessageDialog(LOCTEXT("SNDWidget_GenerateAttributeTable", "Generate attribute table successful."));

	return FReply::Handled();
}

FReply SSimpleNumericalDeductionWidget::DebugDeductionData()
{
	FSimpleNumericalDeductionModule& SimpleNumericalDeductionModul = FModuleManager::LoadModuleChecked<FSimpleNumericalDeductionModule>("SimpleNumericalDeduction");
	SimpleNumericalDeductionModul.SpawnDebugDeductionTable();

	return FReply::Handled();
}

bool SSimpleNumericalDeductionWidget::IsDebugDeductionData() const
{
	return IsGenerateDeduction();
}

bool SSimpleNumericalDeductionWidget::IsGenerateAttributeTable()const
{
	if (const USNDObjectSettings* SND = GetDefault<USNDObjectSettings>())
	{
		if (SND->BaseTable)
		{
			return true;
		}
	}

	return false;
}

FReply SSimpleNumericalDeductionWidget::SaveAsDefault()
{
	if (USNDObjectSettings* SND = const_cast<USNDObjectSettings*>(GetDefault<USNDObjectSettings>()))
	{
		SND->SaveObjectConfig();
	}

	OpenMessageDialog(LOCTEXT("SNDWidget_SaveAsDefault","Storage successful."));

	return FReply::Handled();
}

FReply SSimpleNumericalDeductionWidget::SaveAsCSV()
{
	if (USNDObjectSettings* SND = const_cast<USNDObjectSettings*>(GetDefault<USNDObjectSettings>()))
	{
		for (auto& MainTmp : SND->AttributeDatas)
		{
			FString CSVFilename = SND->CSVSavePath.Path / MainTmp.TableName.ToString() + TEXT(".csv");
			
			TArray<FString> CSVData;
			
			//构建头
			//---,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18
			FString& HeadString = CSVData.AddDefaulted_GetRef();
			HeadString += TEXT("---,");
			for (int32 i = 0; i < SND->DeductionNumber; i++)
			{
				HeadString += FString::FromInt(i + 1) + TEXT(",");
			}
			HeadString.RemoveFromEnd(TEXT(","));

			//构建内容
			//Exp,5,7,14,20,30,40,60,80,110,150,200,260,340,400,559,654,734,890
			//MaxHealth,5,7,14,20,30,40,60,80,110,150,200,260,340,400,559,654,734,890
			for (auto& Tmp : MainTmp.AttributeDatas)
			{
				FString &ContentString = CSVData.AddDefaulted_GetRef();

				//构建Key
				ContentString += Tmp.Key.ToString() + TEXT(",");

				//构建尾数
				for (auto & DeduceTmp : Tmp.DeduceValue)
				{
					ContentString += DeduceTmp + TEXT(",");
				}
				ContentString.RemoveFromEnd(TEXT(","));
			}

			FFileHelper::SaveStringArrayToFile(CSVData,*CSVFilename);
		}

		//打开我们导出的CSV
		FPlatformProcess::ExploreFolder(*SND->CSVSavePath.Path);
	}

	OpenMessageDialog(LOCTEXT("SNDWidget_SaveAsCSV", "Storage csv successful."));

	return FReply::Handled();
}

void SSimpleNumericalDeductionWidget::ClearDeductionValue()
{
	if (USNDObjectSettings* SND = const_cast<USNDObjectSettings*>(GetDefault<USNDObjectSettings>()))
	{
		for (auto& MainTmp : SND->AttributeDatas)
		{
			for (auto& Tmp : MainTmp.AttributeDatas)
			{
				Tmp.DeduceValue.Empty();
			}
		}
	}
}

FReply SSimpleNumericalDeductionWidget::GenerateDeduction()
{
	//清除上次推演的数据
	ClearDeductionValue();

	if (USNDObjectSettings* SND = const_cast<USNDObjectSettings*>(GetDefault<USNDObjectSettings>()))
	{
		FNAEParam NAEParam;

		//存储基础属性值
		for (auto& MainTmp : SND->AttributeDatas)
		{
			for (auto& Tmp : MainTmp.AttributeDatas)
			{
				NAEParam.AttributeData.Add(Tmp.Key.ToString(), Tmp.Value);
			}
		}

		for (auto& MainTmp : SND->AttributeDatas)
		{
			for (auto& Tmp : MainTmp.AttributeDatas)
			{
				if (Tmp.BaseAlgorithm)
				{
					if (UNumericalAlgorithmExecuteObject* InObject =
						Cast<UNumericalAlgorithmExecuteObject>(Tmp.BaseAlgorithm->GetDefaultObject()))
					{
						NAEParam.Key = Tmp.Key.ToString();
						NAEParam.Key.RemoveSpacesInline();//移除本身的空格

						Tmp.DeduceValue.Add(Tmp.Value.Replace(TEXT(","),TEXT("-")));

						//推演具体角色属性
						for (int32 i = 0; i < (SND->DeductionNumber-1); i++)
						{
							NAEParam.Value = FCString::Atof(*Tmp.DeduceValue.Last());
							NAEParam.Count = i+2;
							NAEParam.Coefficient = Tmp.Coefficient;
							
							switch (Tmp.AttributeDataType)
							{
								case EDeduceAttributeDataType::DEDUCETYPE_INT32:
								{
									float InValue = InObject->GetInt32AlgorithmValue(NAEParam);
									Tmp.DeduceValue.Add(FString::SanitizeFloat(InValue));
									break;
								}
								case EDeduceAttributeDataType::DEDUCETYPE_FLOAT:
								{
									int32 InValue = InObject->GetFloatAlgorithmValue(NAEParam);
									Tmp.DeduceValue.Add(FString::FromInt(InValue));
									break;
								}
								case EDeduceAttributeDataType::DEDUCETYPE_STRING:
								{
									Tmp.DeduceValue.Add(InObject->GetStringAlgorithmValue(NAEParam));
									break;
								}
							}
						}
					}
				}
			}
		}
	}

	OpenMessageDialog(LOCTEXT("SNDWidget_GenerateDeduction", "Generate deduction successful."));

	return FReply::Handled();
}

bool SSimpleNumericalDeductionWidget::IsGenerateDeduction()const
{
	if (const USNDObjectSettings* SND = GetDefault<USNDObjectSettings>())
	{
		if (SND->AttributeDatas.Num() && SND->DeductionNumber >= 1)
		{
			return true;
		}
	}

	return false;
}

bool SSimpleNumericalDeductionWidget::IsEnableToCSV() const
{
	if (const USNDObjectSettings* SND = GetDefault<USNDObjectSettings>())
	{
		bool bDeduceValue = false;
		for (auto& MainTmp : SND->AttributeDatas)
		{
			for (auto& Tmp : MainTmp.AttributeDatas)
			{
				if (Tmp.DeduceValue.Num() >= 2)
				{
					bDeduceValue = true;
					break;
				}
			}
		}

		return !SND->CSVSavePath.Path.IsEmpty() && SND->DeductionNumber > 0 && bDeduceValue;
	}

	return false;
}

#undef LOCTEXT_NAMESPACE