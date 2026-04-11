#pragma once

#include "CoreMinimal.h"
#include "UI_Base.h"
#include "UI_ToolTip.generated.h"

class UTextBlock;
class UWidget;
class UUI_ToolTip_ItemInfo;

UCLASS()
class MMOARPG_API UUI_ToolTip : public UUI_Base
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "ToolTip")
	UUI_ToolTip_ItemInfo* WB_ToolTip_ItemInfo;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "ToolTip")
	UWidget* ItemInfo;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "ToolTip")
	UWidget* StatSwitcher;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "ToolTip")
	UWidget* ItemDescription;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "ToolTip")
	UTextBlock* Description;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "ToolTip")
	UWidget* SourceTab;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "ToolTip")
	UWidget* SourceChild;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "ToolTip")
	UWidget* WeightandCurrency;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "ToolTip")
	UTextBlock* Weight;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "ToolTip")
	UTextBlock* price;
};
