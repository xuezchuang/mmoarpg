// Copyright (C) RenZhai.2021.All Rights Reserved.

#include "Widget/Editor/DeduceAttributeCurveTable.h"
#include "UObject/NumbericalAlgorithmCurveObject.h"

#include "CurveEditor.h"
#include "ICurveEditorModule.h"
#include "SCurveEditorPanel.h"
#include "ICurveEditorBounds.h"
#include "Curves/RichCurve.h"
#include "RichCurveEditorModel.h"
#include "Tree/SCurveEditorTree.h"
#include "Tree/CurveEditorTree.h"
#include "Tree/ICurveEditorTreeItem.h"
#include "Tree/SCurveEditorTreeSelect.h"
#include "Tree/SCurveEditorTreePin.h"

#define LOCTEXT_NAMESPACE "FDeduceAttributeCurveTable"

struct FSimpleEditorDACTable
{
	static const FName DeduceAttributeCurveTableID;
};
const FName FSimpleEditorDACTable::DeduceAttributeCurveTableID = TEXT("SimpleEditorDACTable");

struct FSimpleEditorColumnNames
{
	static const FName Label;
	static const FName Select;
	static const FName Pin;
};
const FName FSimpleEditorColumnNames::Label = TEXT("Label");
const FName FSimpleEditorColumnNames::Select = TEXT("Select");
const FName FSimpleEditorColumnNames::Pin = TEXT("Pin");

struct FDACAssetEditorTreeItem : public ICurveEditorTreeItem
{
	FDACAssetEditorTreeItem(TWeakObjectPtr<UCurveBase> InCurveOwner, const FRichCurveEditInfo& InEditInfo);

public:
	virtual TSharedPtr<SWidget> GenerateCurveEditorTreeWidget(const FName& InColumnName, TWeakPtr<FCurveEditor> InCurveEditor, FCurveEditorTreeItemID InTreeItemID, const TSharedRef<ITableRow>& TableRow) override;
	virtual void CreateCurveModels(TArray<TUniquePtr<FCurveModel>>& OutCurveModels) override;
private:
	TWeakObjectPtr<UCurveBase> CurveOwner;
	FRichCurveEditInfo EditInfo;
	FText CurveName;
	FLinearColor CurveColor;
};

FDACAssetEditorTreeItem::FDACAssetEditorTreeItem(TWeakObjectPtr<UCurveBase> InCurveOwner, const FRichCurveEditInfo& InEditInfo)
	: CurveOwner(InCurveOwner)
	, EditInfo(InEditInfo)
{
	if (CurveOwner.IsValid())
	{
		CurveName = FText::FromName(EditInfo.CurveName);

		CurveColor = FLinearColor(
			FMath::RandRange(0.f,1.f),
			FMath::RandRange(0.f, 1.f),
			FMath::RandRange(0.f, 1.f),
			1.f); 

		//CurveOwner->GetCurveColor(EditInfo);
	}
}

TSharedPtr<SWidget> FDACAssetEditorTreeItem::GenerateCurveEditorTreeWidget(const FName& InColumnName, TWeakPtr<FCurveEditor> InCurveEditor, FCurveEditorTreeItemID InTreeItemID, const TSharedRef<ITableRow>& TableRow)
{
	if (InColumnName == FSimpleEditorColumnNames::Label)
	{
		return SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
			.Padding(FMargin(4.f))
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Right)
			.AutoWidth()
			[
				SNew(STextBlock)
				.Text(CurveName)
				.ColorAndOpacity(FSlateColor(CurveColor))
			];
	}
	else if (InColumnName == FSimpleEditorColumnNames::Select)
	{
		return SNew(SCurveEditorTreeSelect, InCurveEditor, InTreeItemID, TableRow);
	}
	else if (InColumnName == FSimpleEditorColumnNames::Pin)
	{
		return SNew(SCurveEditorTreePin, InCurveEditor, InTreeItemID, TableRow);
	}

	return NULL;
}

void FDACAssetEditorTreeItem::CreateCurveModels(TArray<TUniquePtr<FCurveModel>>& OutCurveModels)
{
	if (!CurveOwner.IsValid())
	{
		return;
	}

	TUniquePtr<FRichCurveEditorModelRaw> NewCurve = MakeUnique<FRichCurveEditorModelRaw>(static_cast<FRichCurve*>(EditInfo.CurveToEdit), CurveOwner.Get());
	NewCurve->SetShortDisplayName(CurveName);
	NewCurve->SetColor(CurveColor);

	OutCurveModels.Add(MoveTemp(NewCurve));
}

FSDeduceAttributeCurveTable::FSDeduceAttributeCurveTable()
	:DeduceAttributeDataTables(NULL)
	,DeduceAttributeDataTable(NULL)
{
}

void FSDeduceAttributeCurveTable::InitLayout()
{
	//if (bInitialization)
	//{
	//	return;
	//}

	//bInitialization = true;

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(FSimpleEditorDACTable::DeduceAttributeCurveTableID);

	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(FSimpleEditorDACTable::DeduceAttributeCurveTableID, FOnSpawnTab::CreateRaw(this, &FSDeduceAttributeCurveTable::SpawnTab_CurveAsset))
		.SetDisplayName(LOCTEXT("FSimpleEditorDACTableTitle", "DeduceAttributeCurveTable"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);

	TSharedRef<FTabManager::FLayout> Layout = FTabManager::NewLayout("Simple_DeduceAttributeCurve_Layout")
		->AddArea
		(
			FTabManager::NewArea(640, 800)

			->Split
			(
				FTabManager::NewStack()
				->AddTab(FSimpleEditorDACTable::DeduceAttributeCurveTableID, ETabState::OpenedTab)
			)
		);

	FGlobalTabmanager::Get()->RestoreFrom(Layout, TSharedPtr<SWindow>());
}

void FSDeduceAttributeCurveTable::Construct(FDeduceAttributeDataTables& InDeduceAttributeDataTables)
{
	DeduceAttributeDataTables = &InDeduceAttributeDataTables;
	DeduceAttributeDataTable = NULL;

	//TSharedPtr<SDockTab> DockTab = FGlobalTabmanager::Get()->FindExistingLiveTab(FSimpleEditorDACTable::DeduceAttributeCurveTableID);
	//if (DockTab.IsValid())
	//{
	//	FGlobalTabmanager::Get()->TryInvokeTab(FSimpleEditorDACTable::DeduceAttributeCurveTableID);
	//}
}

void FSDeduceAttributeCurveTable::Construct(FDeduceAttributeData& InDeduceAttributeDataTable)
{
	DeduceAttributeDataTable = &InDeduceAttributeDataTable;
	DeduceAttributeDataTables = NULL;

	//TSharedPtr<SDockTab> DockTab = FGlobalTabmanager::Get()->FindExistingLiveTab(FSimpleEditorDACTable::DeduceAttributeCurveTableID);
	//if (DockTab.IsValid())
	//{
	//	FGlobalTabmanager::Get()->TryInvokeTab(FSimpleEditorDACTable::DeduceAttributeCurveTableID);
	//}
}

void ConstructDeduceAttribute(FRichCurveEditInfo& InCurveEditInfo, FDeduceAttributeData* InDeduceAttributeDataTable)
{
	if (InDeduceAttributeDataTable)
	{
		InCurveEditInfo.CurveName = InDeduceAttributeDataTable->Key;
		InCurveEditInfo.CurveToEdit = new FRichCurve();
		InCurveEditInfo.CurveToEdit->SetDefaultValue(0.f);

		for (int32 i = 0; i < InDeduceAttributeDataTable->DeduceValue.Num(); i++)
		{
			InCurveEditInfo.CurveToEdit->UpdateOrAddKey(i,FCString::Atof(*InDeduceAttributeDataTable->DeduceValue[i]));
		}
	}
}

TSharedRef<SDockTab> FSDeduceAttributeCurveTable::SpawnTab_CurveAsset(const FSpawnTabArgs& Args)
{
	CurveEditor = MakeShared<FCurveEditor>();

	CurveEditor->GridLineLabelFormatXAttribute = LOCTEXT("GridXLabelFormat", "{0}");

	TUniquePtr<ICurveEditorBounds> EditorBounds = MakeUnique<FStaticCurveEditorBounds>();
	EditorBounds->SetInputBounds(-1.05, 1.05);

	CurveEditorPanel = SNew(SCurveEditorPanel, CurveEditor.ToSharedRef())
	.TreeContent()
	[
		SNew(SCurveEditorTree, CurveEditor)
	];

	auto CreateMyCurves = [&](UNumbericalAlgorithmCurveObject* InCurve)
	{
		check(CurveEditor.IsValid());
		if (InCurve->HasRichCurves())
		{
			for (const FRichCurveEditInfo& CurveData : InCurve->GetCurves())
			{
				TSharedPtr<FDACAssetEditorTreeItem> TreeItem = MakeShared<FDACAssetEditorTreeItem>(InCurve, CurveData);

				FCurveEditorTreeItem* NewItem = CurveEditor->AddTreeItem(FCurveEditorTreeItemID::Invalid());
				NewItem->SetStrongItem(TreeItem);

				for (const FCurveModelID& CurveModel : NewItem->GetOrCreateCurves(CurveEditor.Get()))
				{
					CurveEditor->PinCurve(CurveModel);
				}
			}
		}
	};

	//生成我们的单个曲线
	if (DeduceAttributeDataTable)
	{
		if (UNumbericalAlgorithmCurveObject* Curve = NewObject<UNumbericalAlgorithmCurveObject>())
		{
			FRichCurveEditInfo EditInfo;
			ConstructDeduceAttribute(EditInfo, DeduceAttributeDataTable);

			Curve->AddCurves(EditInfo);

			CreateMyCurves(Curve);
		}
	}
	//生成我们的多个曲线
	else if (DeduceAttributeDataTables)
	{
		if (UNumbericalAlgorithmCurveObject* Curve = NewObject<UNumbericalAlgorithmCurveObject>())
		{
			for (auto& Tmp : DeduceAttributeDataTables->AttributeDatas)
			{
				FRichCurveEditInfo EditInfo;
				ConstructDeduceAttribute(EditInfo,&Tmp);

				Curve->AddCurves(EditInfo);
			}

			CreateMyCurves(Curve);
		}
	}

	TSharedRef<SDockTab> NewDockTab = SNew(SDockTab)
		[
			SNew(SBorder)
			.BorderImage(FAppStyle::Get().GetBrush("ToolPanel.GroupBorder"))
			.Padding(0.0f)
			[
				CurveEditorPanel.ToSharedRef()
			]
		];
	NewDockTab->SetTabIcon(FAppStyle::Get().GetBrush("CurveAssetEditor.Tabs.Properties"));
	return NewDockTab;
}

#undef LOCTEXT_NAMESPACE