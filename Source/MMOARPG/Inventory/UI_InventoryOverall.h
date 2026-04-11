#pragma once

#include "CoreMinimal.h"
#include "UI_Base.h"
#include "UI_InventoryOverall.generated.h"

class UButton;
class UWidget;
class UWidgetSwitcher;
class UUI_DropBar;
class UUI_GoldWeightBar;
class UUI_InventoryBase;
class UUI_Stats;

UCLASS()
class MMOARPG_API UUI_InventoryOverall : public UUI_Base
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "InventoryOverall")
	UWidgetSwitcher* WidgetSwitcher;


	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "InventoryOverall")
	UUI_GoldWeightBar* WB_GoldWeightBar;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "InventoryOverall")
	UUI_InventoryBase* WB_InventoryBase;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "InventoryOverall")
	UUI_DropBar* WB_DropBar;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "InventoryOverall")
	UWidget* WB_Equipment;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "InventoryOverall")
	UUI_Stats* WB_Stats;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "InventoryOverall")
	UButton* Button_CRAFT_Continue;

public:
	UFUNCTION(BlueprintCallable, Category = "InventoryOverall")
	void RefreshInventoryWidgets();
};
