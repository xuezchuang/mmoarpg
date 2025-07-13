// Copyright (C) RenZhai.2021.All Rights Reserved.

#include "SimpleNumericalDeduction.h"
#include "SimpleNumericalDeductionStyle.h"
#include "SimpleNumericalDeductionCommands.h"
#include "LevelEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "ToolMenus.h"
#include "Widget/SSimpleNumericalDeductionWidget.h"
#include "DetailsMapping/SimpleSelectStringDetail.h"
#include "PropertyEditorModule.h"

static const FName SimpleNumericalDeductionTabName("SimpleNumericalDeduction");

#define LOCTEXT_NAMESPACE "FSimpleNumericalDeductionModule"

void FSimpleNumericalDeductionModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FSimpleNumericalDeductionStyle::Initialize();
	FSimpleNumericalDeductionStyle::ReloadTextures();

	FSimpleNumericalDeductionCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FSimpleNumericalDeductionCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FSimpleNumericalDeductionModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FSimpleNumericalDeductionModule::RegisterMenus));
	
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(SimpleNumericalDeductionTabName, FOnSpawnTab::CreateRaw(this, &FSimpleNumericalDeductionModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FSimpleNumericalDeductionTabTitle", "SimpleNumericalDeduction"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);

	//自定义对象
	RegisterObjectCustomizations();
	//自定义变量布局
	RegisterPropertyTypeCustomizations();
}

void FSimpleNumericalDeductionModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FSimpleNumericalDeductionStyle::Shutdown();

	FSimpleNumericalDeductionCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(SimpleNumericalDeductionTabName);

	//卸载
	if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
	{
		FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
		for (auto It = RegisteredPropertyTypes.CreateConstIterator(); It; ++It)
		{
			if (It->IsValid())
			{
				PropertyModule.UnregisterCustomPropertyTypeLayout(*It);
			}
		}

		PropertyModule.NotifyCustomizationModuleChanged();
	}
}

TSharedRef<SDockTab> FSimpleNumericalDeductionModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	return SNew(SDockTab)
	.TabRole(ETabRole::NomadTab)
	[
		// Put your tab content here!
		SNew(SBox)
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SNew(SSimpleNumericalDeductionWidget)
		]
	];
}

void FSimpleNumericalDeductionModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(SimpleNumericalDeductionTabName);
}

void FSimpleNumericalDeductionModule::SpawnAttributeCurveTable(FDeduceAttributeDataTables& InDeduceAttributeDataTables)
{
	AttributeCurveTable.Construct(InDeduceAttributeDataTables);
	
	//初始化曲线编辑器
	AttributeCurveTable.InitLayout();
}

void FSimpleNumericalDeductionModule::SpawnAttributeCurveTable(FDeduceAttributeData& InDeduceAttributeDataTabl)
{
	AttributeCurveTable.Construct(InDeduceAttributeDataTabl);

	//初始化曲线编辑器
	AttributeCurveTable.InitLayout();
}

void FSimpleNumericalDeductionModule::SpawnDebugDeductionTable()
{
	DebugAttibuteDeduceTable.Construct();

	//初始化我们当Debug界面布局
	DebugAttibuteDeduceTable.InitLayout();
}

void FSimpleNumericalDeductionModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FSimpleNumericalDeductionCommands::Get().OpenPluginWindow, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Settings");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FSimpleNumericalDeductionCommands::Get().OpenPluginWindow));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

void FSimpleNumericalDeductionModule::RegisterPropertyTypeCustomizations()
{
	RegisterCustomPropertyTypeLayout("SimpleSelectString", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FSimpleSelectStringDetail::MakeInstance));
}

void FSimpleNumericalDeductionModule::RegisterCustomPropertyTypeLayout(FName PropertyTypeName, FOnGetPropertyTypeCustomizationInstance PropertyTypeLayoutDelegate)
{
	check(PropertyTypeName != NAME_None);

	RegisteredPropertyTypes.Add(PropertyTypeName);

	FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.RegisterCustomPropertyTypeLayout(PropertyTypeName, PropertyTypeLayoutDelegate);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSimpleNumericalDeductionModule, SimpleNumericalDeduction)