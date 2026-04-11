#pragma once

#include "CoreMinimal.h"
#include "UI_Base.h"
#include "Engine\Texture2D.h"
#include "Data/FPlayerInventoryData.h"
#include "UI_InventorySlot.h"

#include "UI_InventoryBase.generated.h"

class UButton;
class UImage;
class UUI_CategoryButton;
class UUI_ToolTip;
class UUniformGridPanel;
class UWidgetSwitcher;

UCLASS()
class UUI_InventoryBase : public UUI_Base
{
	GENERATED_BODY()
#pragma region UI_CategoryButton

	UPROPERTY(meta = (BindWidget))
	UUI_CategoryButton* WB_CategoryWeapon;

	UPROPERTY(meta = (BindWidget))
	UUI_CategoryButton* WB_CategoryRange;

	UPROPERTY(meta = (BindWidget))
	UUI_CategoryButton* WB_CategoryArmor;

	UPROPERTY(meta = (BindWidget))
	UUI_CategoryButton* WB_CategoryAccessories;

	UPROPERTY(meta = (BindWidget))
	UUI_CategoryButton* WB_CategoryRuneStone;

	UPROPERTY(meta = (BindWidget))
	UUI_CategoryButton* WB_CategoryConsumable;

	UPROPERTY(meta = (BindWidget))
	UUI_CategoryButton* WB_CategoryResources;

	UPROPERTY(meta = (BindWidget))
	UUI_CategoryButton* WB_CategoryMount;

	UPROPERTY(meta = (BindWidget))
	UUI_CategoryButton* WB_CategoryQuest;

	UPROPERTY(meta = (BindWidget))
	UUI_CategoryButton* WB_CategoryEvent;
#pragma endregion

	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher* Panels_Switcher;

	UPROPERTY(meta = (BindWidgetOptional))
	UUI_ToolTip* WB_CompareToolTip;

	UPROPERTY(meta = (BindWidgetOptional))
	UUI_ToolTip* WB_MainToolTip;

#pragma region UniformGridPanels
	UPROPERTY(meta = (BindWidget))
	UUniformGridPanel* UniformGrid_P1;

	UPROPERTY(meta = (BindWidget))
	UUniformGridPanel* UniformGrid_P2;

	UPROPERTY(meta = (BindWidget))
	UUniformGridPanel* UniformGrid_P3;

	UPROPERTY(meta = (BindWidget))
	UUniformGridPanel* UniformGrid_P4;

	UPROPERTY(meta = (BindWidget))
	UUniformGridPanel* UniformGrid_P5;

	UPROPERTY(meta = (BindWidget))
	UUniformGridPanel* UniformGrid_P6;

	UPROPERTY(meta = (BindWidget))
	UUniformGridPanel* UniformGrid_P7;

	UPROPERTY(meta = (BindWidget))
	UUniformGridPanel* UniformGrid_P8;

	UPROPERTY(meta = (BindWidget))
	UUniformGridPanel* UniformGrid_P9;

	UPROPERTY(meta = (BindWidget))
	UUniformGridPanel* UniformGrid_P10;
#pragma endregion

public:
	virtual void NativeConstruct();
	virtual void NativeDestruct();
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	void PopulateGridForCategory(E_InventoryCategory Category);
	void RefreshCurrentCategory();
	UUniformGridPanel* GetGridByCategory(E_InventoryCategory Category) const;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TSubclassOf<UUI_InventorySlot> InventorySlotClass;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	int32 TotalSlots = 40;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	int32 Columns = 5;

	/** 自动列数上限。<=0 表示不限制 */
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	int32 MaxAutoColumns = 8;

	/** 每格最小宽度（像素），控制 Grid 撑满容器，按实际槽位宽度调整 */
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	float SlotMinWidth = 90.f;

	/** 每格最小高度（像素） */
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	float SlotMinHeight = 110.f;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	bool bClearEmptySlotsVisual = false;

protected:
	void NativePreConstruct() override;

private:
	int32 ResolveRuntimeColumns(UUniformGridPanel* Grid) const;
	void UpdateSelectedSlot(UUI_InventorySlot* SlotWidget, const FFS_ItemData* ItemData);
	void UpdateMainToolTip(const FFS_ItemData* ItemData) const;
	void ClearSelection();
	void HandleSlotClicked(UUI_InventorySlot* SlotWidget, const FFS_ItemData* ItemData);

	E_InventoryCategory CurrentCategory = E_InventoryCategory::Weapon;
	int32 RuntimeColumns = 0;
	bool bRebuildingFromAutoColumns = false;
	TWeakObjectPtr<UUI_InventorySlot> SelectedSlotWidget;
	const FFS_ItemData* SelectedItemData = nullptr;

	UFUNCTION() void OnCategoryWeapon();
	UFUNCTION() void OnCategoryRange();
	UFUNCTION() void OnCategoryArmor();
	UFUNCTION() void OnCategoryAccessories();
	UFUNCTION() void OnCategoryRuneStone();
	UFUNCTION() void OnCategoryConsumable();
	UFUNCTION() void OnCategoryResources();
	UFUNCTION() void OnCategoryMount();
	UFUNCTION() void OnCategoryQuest();
	UFUNCTION() void OnCategoryEvent();

	void SetActiveCategoryButton(UUI_CategoryButton* ActiveBtn);
};
