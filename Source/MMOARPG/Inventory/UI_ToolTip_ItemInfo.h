#pragma once

#include "CoreMinimal.h"
#include "UI_Base.h"
#include "UI_ToolTip_ItemInfo.generated.h"

class UImage;
class UTextBlock;
class UUniformGridPanel;
class UWidget;
class UWidgetSwitcher;
class UUI_ConsumableTypeIcon;

UCLASS()
class MMOARPG_API UUI_ToolTip_ItemInfo : public UUI_Base
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "ToolTipItemInfo")
	UTextBlock* ItemName;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "ToolTipItemInfo")
	UWidgetSwitcher* ToolTipSwitcher;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "ToolTipItemInfo")
	UUI_ConsumableTypeIcon* WB_Icon_Consumable_Type;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "ToolTipItemInfo")
	UTextBlock* ItemClassText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "ToolTipItemInfo")
	UTextBlock* N_SecondaryStat;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "ToolTipItemInfo")
	UTextBlock* T_SecondaryStat;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "ToolTipItemInfo")
	UTextBlock* N_MainStat;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "ToolTipItemInfo")
	UTextBlock* T_MainStat;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "ToolTipItemInfo")
	UUniformGridPanel* CloneGridBoost;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "ToolTipItemInfo")
	UImage* ItemIcon;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "ToolTipItemInfo")
	UWidget* Equipped;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "ToolTipItemInfo")
	UTextBlock* CompareText;
};
