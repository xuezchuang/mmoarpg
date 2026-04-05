#pragma once

#include "CoreMinimal.h"
#include "UI_Base.h"
#include "../Data/FItemData.h"
#include "UI_SplitStack.h"
#include "../Core/Game/MMOARPGPlayerController.h"
#include "UI_VendorStorageInventory.generated.h"

class UUI_CategoryButton;
class UUniformGridPanel;
class UUI_VendorStorageSlot;
class UInteractionComponent;

UCLASS()
class UUI_VendorStorageInventory : public UUI_Base
{
	GENERATED_BODY()

	//UPROPERTY(meta = (BindWidget))
	//UUI_CategoryButton* WB_VendorBuy;

	//UPROPERTY(meta = (BindWidget))
	//UUI_CategoryButton* WB_VendorSellBack;

public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UUniformGridPanel* StorageUniformGrid;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UUniformGridPanel* UniformGridBuyBack;

	UPROPERTY(EditDefaultsOnly, Category = "BP_Setting")
	TSubclassOf<UUI_VendorStorageSlot> Slot_BPClass;

	/** 购买弹窗蓝图类，在 Blueprint 子类的 Details 里赋值 */
	UPROPERTY(EditDefaultsOnly, Category = "BP_Setting")
	TSubclassOf<UUI_SplitStack> SplitStackClass;

	UPROPERTY(BlueprintReadWrite, Category = "BP_Setting")
	UInteractionComponent* InteractionComponent = NULL;
	//protected:
public:
	void UpdateInteraction(UInteractionComponent* ITCom);

	/** R 键"返回"逻辑：SplitStack 可见时关闭弹窗，否则返回 false 让 Vendor 自己关闭 */
	bool TryCloseSplitStack();

	UFUNCTION(BlueprintImplementableEvent, Category = "BP_Setting")
	void UpdateItem(const FFS_ItemData& ItemData);

	//UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	//void UnOnFocus();
public:
	virtual void NativeConstruct();

	virtual void NativeDestruct();
private:

	void DeleteUpdateItem(const FFS_ItemData* ItemData);

	void InitItems();

	const FFS_ItemData* m_pItemData = nullptr;
	TMap<const FFS_ItemData*, UUI_VendorStorageSlot*> mapSlot;

	/** 复用的购买弹窗实例 */
	UPROPERTY()
	UUI_SplitStack* m_SplitStack = nullptr;

	FDelegateHandle m_VendorHotkeyHandle;

	void OnVendorHotkey(ESystemHotkey Action);
	void OpenSplitStack();
};
