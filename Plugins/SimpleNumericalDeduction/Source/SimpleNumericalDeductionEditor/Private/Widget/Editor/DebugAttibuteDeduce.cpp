// Copyright (C) RenZhai.2021.All Rights Reserved.
#include "Widget/Editor/DebugAttibuteDeduce.h"
#include "Widget/Slate/SDebugDataTableAttributeTable.h"
#include "Log/SimpleNumericalDeductionLog.h"
#include "EditorStyle/Public/EditorStyleSet.h"

#define LOCTEXT_NAMESPACE "FDebugAttibuteDeduce"

struct FSimpleEditorDADTable
{
	static const FName DebugAttibuteDeduceTableID;
};

const FName FSimpleEditorDADTable::DebugAttibuteDeduceTableID = TEXT("DebugAttibuteDeduce");

FDebugAttibuteDeduce::FDebugAttibuteDeduce()
{

}

void FDebugAttibuteDeduce::InitLayout()
{
	//if (bInitialization)
	//{
	//	return;
	//}

	//bInitialization = true;

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(FSimpleEditorDADTable::DebugAttibuteDeduceTableID);

	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(FSimpleEditorDADTable::DebugAttibuteDeduceTableID, FOnSpawnTab::CreateRaw(this, &FDebugAttibuteDeduce::SpawnTab_DebugSettingWidget))
		.SetDisplayName(LOCTEXT("FDebugAttibuteDeduceSettingsTitle", "DebugAttibuteDeduceSettingsTable"));

	TSharedRef<FTabManager::FLayout> Layout = FTabManager::NewLayout("Simple_DeduceSettings_Layout")
		->AddArea
		(
			FTabManager::NewArea(640, 800)
			->Split
			(
				FTabManager::NewStack()
				->AddTab(FSimpleEditorDADTable::DebugAttibuteDeduceTableID, ETabState::OpenedTab)
			)
		);

	FGlobalTabmanager::Get()->RestoreFrom(Layout, TSharedPtr<SWindow>());
}

void FDebugAttibuteDeduce::Construct()
{
	//FGlobalTabmanager::Get()->TryInvokeTab(FSimpleEditorDADTable::DebugAttibuteDeduceTableID);
}

TSharedRef<SDockTab> FDebugAttibuteDeduce::SpawnTab_DebugSettingWidget(const FSpawnTabArgs& Args)
{
	TSharedRef<SDockTab> NewDockTab = SNew(SDockTab)
		[
			SNew(SBorder)
			.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
			.Padding(0.0f)
			[
				SNew(SDebugDataTableAttributeTable)
			]
		];
	NewDockTab->SetTabIcon(FAppStyle::Get().GetBrush("CurveAssetEditor.Tabs.Properties"));
	return NewDockTab;
}

#undef LOCTEXT_NAMESPACE