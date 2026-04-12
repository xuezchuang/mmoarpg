#pragma once

#include "CoreMinimal.h"
#include "UI_Base.h"
#include "../Data/FItemData.h"
#include "../Core/Game/MMOARPGPlayerController.h"
#include "UI_SplitStack.generated.h"

class UTextBlock;
class UButton;
class USlider;
class UEditableTextBox;
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
	UTextBlock* MaxCountText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* CostText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* WeightText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* GoldText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* DecreaseValueButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* IncreaseValueButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	USlider* Head_Slider;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UEditableTextBox* EditableTextBox_233;

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual bool NativeSupportsKeyboardFocus() const override { return true; }
	virtual FReply NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	/** 打开时初始化物品数据，同时订阅 PlayerController 的 Vendor 快捷键 */
	void InitWithItem(const FFS_ItemData* Item);

private:
	void AddCount(int32 Delta);
	void SetCount(int32 NewCount);
	void ConfirmBuy();
	void RefreshUI();
	void RecvProtocol(uint32 ProtocolNumber);
	void BindNet();
	void UnbindNet();
	uint32 ResolveBuyPropId() const;
	void BindInputWidgets();
	void UnbindInputWidgets();
	void FocusSplitStack();
	void CloseSplitStack();

	UFUNCTION()
	void OnDecreaseValueButtonClicked();

	UFUNCTION()
	void OnIncreaseValueButtonClicked();

	UFUNCTION()
	void OnHeadSliderValueChanged(float InValue);

	UFUNCTION()
	void OnCountTextCommitted(const FText& InText, ETextCommit::Type InCommitMethod);

	/** 响应 PlayerController 广播的 Vendor 快捷键 */
	void OnVendorHotkey(ESystemHotkey Action);

	void BindHotkeys();
	void UnbindHotkeys();

private:
	const FFS_ItemData* m_ItemData  = nullptr;
	int32               m_Count     = 1;
	int32               m_MaxCount  = 99;
	bool                m_bPendingBuy = false;
	bool                m_bRefreshingUI = false;
	int32               m_PendingCount = 0;
	uint32              m_PendingPropId = 0;
	FFS_ItemData        m_PendingItemData;

	FDelegateHandle     m_HotkeyHandle;
	TArray<uint32>      m_InterestedProtos;
	TArray<FDelegateHandle> m_ProtoHandles;
};
