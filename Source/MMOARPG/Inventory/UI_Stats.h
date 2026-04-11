#pragma once

#include "CoreMinimal.h"
#include "UI_Base.h"
#include "UI_Stats.generated.h"

class UProgressBar;
class UTextBlock;
class UUniformGridPanel;
class UWidget;
class UWidgetSwitcher;
class UUI_Clone_Boost;

UCLASS()
class MMOARPG_API UUI_Stats : public UUI_Base
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "Stats")
	UWidgetSwitcher* WidgetSwitcher_0;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "Stats")
	UWidget* PlayerStat;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "Stats")
	UTextBlock* hptext;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "Stats")
	UProgressBar* hpbar;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "Stats")
	UTextBlock* MPText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "Stats")
	UProgressBar* MPBar;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "Stats")
	UTextBlock* SPText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "Stats")
	UProgressBar* SPBar;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "Stats")
	UTextBlock* DMGText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "Stats")
	UTextBlock* AttackSpeedText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "Stats")
	UTextBlock* RangeDamageText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "Stats")
	UTextBlock* RangeAttackSpeedText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "Stats")
	UTextBlock* ArmorText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "Stats")
	UTextBlock* CriticalRateText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "Stats")
	UTextBlock* CriticalDamageText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "Stats")
	UWidget* Main_ActiveWeaponIndicator;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "Stats")
	UWidget* Sub_ActiveWeaponIndicator;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "Stats")
	UWidget* DetailStats;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "Stats")
	UWidget* HorseStat;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "Stats")
	UTextBlock* MountClass;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "Stats")
	UUniformGridPanel* CloneGridBoost;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "Stats")
	UUI_Clone_Boost* WBP_Clone_Boost;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "Stats")
	UTextBlock* Acceleration;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "Stats")
	UProgressBar* AccelerationBar;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "Stats")
	UTextBlock* Decceleration;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "Stats")
	UProgressBar* DeccelerationBar;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "Stats")
	UTextBlock* SR;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "Stats")
	UProgressBar* SRBar;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "Stats")
	UTextBlock* MaxSpeed;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "Stats")
	UTextBlock* MaxStaminal;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "Stats")
	UTextBlock* SSC;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "Stats")
	UTextBlock* FBR;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "Stats")
	UTextBlock* MD;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "Stats")
	UTextBlock* MSC;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "Stats")
	UTextBlock* MT;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "Stats")
	UTextBlock* AS;
};
