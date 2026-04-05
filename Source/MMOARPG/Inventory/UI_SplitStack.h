#pragma once

#include "CoreMinimal.h"
#include "UI_Base.h"
#include "../Data/FItemData.h"
#include "../Core/Game/MMOARPGPlayerController.h"
#include "UI_SplitStack.generated.h"

class UTextBlock;
struct FFS_ItemData;

UCLASS()
class UUI_SplitStack : public UUI_Base
{
	GENERATED_BODY()

public:
	// ---- BindWidget（名称需与蓝图中控件一致） ----

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* ItemNameText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* CountText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* MaxCountText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* CostText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* WeightText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* GoldText;

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	/** 打开时初始化物品数据，同时订阅 PlayerController 的 Vendor 快捷键 */
	void InitWithItem(const FFS_ItemData* Item);

private:
	void AddCount(int32 Delta);
	void ConfirmBuy();
	void RefreshUI();

	/** 响应 PlayerController 广播的 Vendor 快捷键 */
	void OnVendorHotkey(ESystemHotkey Action);

	void BindHotkeys();
	void UnbindHotkeys();

private:
	const FFS_ItemData* m_ItemData  = nullptr;
	int32               m_Count     = 1;
	int32               m_MaxCount  = 99;

	FDelegateHandle     m_HotkeyHandle;
};
