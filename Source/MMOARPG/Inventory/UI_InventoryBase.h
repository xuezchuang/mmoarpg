#pragma once

#include "CoreMinimal.h"
#include "UI_Base.h"
#include "Engine\Texture2D.h"
#include "Data/FPlayerInventoryData.h"

#include "UI_InventoryBase.generated.h"

class UButton;
class UImage;
class UUI_CategoryButton;
class UUniformGridPanel;
//class UUserWidget;

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

	/** 将指定分类的物品填充到对应的 UniformGrid（打开背包时调用） */
	void PopulateGridForCategory(E_InventoryCategory Category);

	/** 根据分类枚举获取对应的 UniformGrid 面板 */
	UUniformGridPanel* GetGridByCategory(E_InventoryCategory Category) const;

	/** 背包物品槽 Widget 蓝图类，需在 Blueprint 中赋值 */
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TSubclassOf<UUserWidget> InventorySlotClass;

protected:
	void NativePreConstruct() override;

};
