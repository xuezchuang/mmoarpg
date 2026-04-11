#pragma once

#include "CoreMinimal.h"
#include "UI_Base.h"
#include "../Data/FItemData.h"
#include "UI_InventorySlot.generated.h"

class UImage;
class UTextBlock;
class UButton;
class UUniformGridPanel;
class UWidget;
class UUI_RarityStar;
class UUI_ConsumableTypeIcon;

DECLARE_DELEGATE_OneParam(FOnInventorySlotClicked, const FFS_ItemData*);

/**
 * UI_InventorySlot — 背包物品格 Widget
 *
 * Blueprint 子类须绑定以下控件（名称必须一致）：
 *   RarityColor       Image            — 稀有度底色
 *   BrokenImage       Image            — 损坏状态图
 *   Highlight         Image            — 高亮遮罩
 *   Black_Gradient    Image            — 渐变遮罩
 *   Image_Item        Image            — 物品图标
 *   Quantity          TextBlock        — 数量文字
 *   CloneGridBoost    UniformGridPanel — 稀有度星星容器
 *   Hover             Image            — Hover 遮罩
 *   ActiveBorder      Image            — 选中边框
 *   SellQuantity      TextBlock        — 商人出售数量 "x1"（可选）
 *   Item_Button       Button           — 点击按钮
 *   MerchantItem      Widget           — 商人专属区域（可选）
 */
UCLASS()
class MMOARPG_API UUI_InventorySlot : public UUI_Base
{
	GENERATED_BODY()

	// ---- 稀有度底色 ----
	UPROPERTY(meta = (BindWidget))
	UImage* RarityColor;

	UPROPERTY(meta = (BindWidget))
	UImage* BrokenImage;

	UPROPERTY(meta = (BindWidget))
	UImage* Highlight;

	UPROPERTY(meta = (BindWidget))
	UImage* Black_Gradient;

	// ---- 物品图标 ----
	UPROPERTY(meta = (BindWidget))
	UImage* Image_Item;

	UPROPERTY(meta = (BindWidgetOptional))
	UUI_ConsumableTypeIcon* WB_Icon_Consumable_Type;

	// ---- 数量 ----
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Quantity;

	// ---- 稀有度星星 UniformGrid ----
	UPROPERTY(meta = (BindWidget))
	UUniformGridPanel* CloneGridBoost;

	// ---- 悬停 / 激活 ----
	UPROPERTY(meta = (BindWidget))
	UImage* Hover;

	UPROPERTY(meta = (BindWidget))
	UImage* ActiveBorder;

	// ---- 商人模式（可选绑定）----
	UPROPERTY(meta = (BindWidgetOptional))
	UWidget* MerchantItem;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* SellQuantity;

	// ---- 点击按钮 ----
	UPROPERTY(meta = (BindWidget))
	UButton* Item_Button;

public:
	/** 蓝图里指定 UI_RarityStar 的子类，用于动态创建星星 */
	UPROPERTY(EditDefaultsOnly, Category = "InventorySlot")
	TSubclassOf<UUI_RarityStar> RarityStarClass;

	/** 当格子被点击时广播 */
	FOnInventorySlotClicked OnSlotClicked;

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	/** 设置物品数据，刷新所有 UI 显示 */
	void SetItemData(const FFS_ItemData* InItemData);

	/** 设置商人模式（显示/隐藏 MerchantItem，更新 SellQuantity 文字） */
	void SetMerchantMode(bool bMerchant, int32 InSellQuantity = 1);

	/** 设置选中高亮状态 */
	void SetActive(bool bActive);

private:
	const FFS_ItemData* m_ItemData = nullptr;

	UFUNCTION()
	void OnButtonClicked();

	/** 根据稀有度等级刷新 CloneGridBoost 中的星星 */
	void RefreshRarityStars(E_ItemRarity Rarity);
};
