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
class UUI_ToolTip;
class UTextBlock;

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

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	UUI_ToolTip* WB_MainToolTip;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	UTextBlock* OwnedText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	UTextBlock* TextGold;

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
	virtual bool NativeSupportsKeyboardFocus() const override { return true; }
	virtual FReply NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	void FocusVendorPanel();
	void NotifyPurchaseSucceeded(const FFS_ItemData* ItemData, int32 PurchasedCount);

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
	void UpdateMainToolTip(const FFS_ItemData* ItemData) const;
	void UpdateOwnedText(const FFS_ItemData* ItemData) const;
	void UpdateGoldText();
	void RecvProtocol(uint32 ProtocolNumber);
	void ClearSelection();
	int32 GetOwnedItemCount(const FFS_ItemData* ItemData) const;

	void InitItems();

	const FFS_ItemData* m_pItemData = nullptr;
	TMap<const FFS_ItemData*, UUI_VendorStorageSlot*> mapSlot;

	/** 复用的购买弹窗实例 */
	UPROPERTY()
	UUI_SplitStack* m_SplitStack = nullptr;

	FDelegateHandle m_VendorHotkeyHandle;
	TArray<uint32> InterestingProtos;
	TArray<FDelegateHandle> InterestingHandles;

	void OnVendorHotkey(ESystemHotkey Action);
	void OpenSplitStack();
};
