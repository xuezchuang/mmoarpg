// Copyright (C) RenZhai.2021.All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widget/Editor/DeduceAttributeCurveTable.h"
#include "Widget/Editor/DebugAttibuteDeduce.h"
#include "Modules/ModuleManager.h"

class FToolBarBuilder;
class FMenuBuilder;

class FSimpleNumericalDeductionModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	/** This function will be bound to Command (by default it will bring up plugin window) */
	void PluginButtonClicked();
public:
	//�������߱�
	void SpawnAttributeCurveTable(FDeduceAttributeDataTables& InDeduceAttributeDataTables);
	void SpawnAttributeCurveTable(FDeduceAttributeData& InDeduceAttributeDataTabl);
	
	//���ɵ��Ա�
	void SpawnDebugDeductionTable();
private:
	void RegisterObjectCustomizations(){}
	void RegisterPropertyTypeCustomizations();

	void RegisterCustomPropertyTypeLayout(FName PropertyTypeName, FOnGetPropertyTypeCustomizationInstance PropertyTypeLayoutDelegate);
private:
	void RegisterMenus();

	TSharedRef<class SDockTab> OnSpawnPluginTab(const class FSpawnTabArgs& SpawnTabArgs);

private:
	TSharedPtr<class FUICommandList> PluginCommands;

	//���߱༭��
	FSDeduceAttributeCurveTable AttributeCurveTable;
	FDebugAttibuteDeduce DebugAttibuteDeduceTable;

private:
	TSet< FName > RegisteredPropertyTypes;
};
