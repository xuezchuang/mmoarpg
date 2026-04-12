// Fill out your copyright notice in the Description page of Project Settings.

#include "UI_VendorStorageInventory.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/TextBlock.h"
#include "Components/UniformGridPanel.h"
#include "UI_ToolTip.h"
#include "UI_VendorStorageSlot.h"
#include "../MMOARPG.h"
#include "../Core/Common/MMOARPGGameInstance.h"
#include "../Core/Common/MMOARPGNetSubsystem.h"
#include "../Core/Component/InventoryComponent.h"
#include "../Core/Component/InteractionComponent.h"
#include "../Core/Game/MMOARPGGameState.h"
#include "../Core/Game/MMOARPGPlayerState.h"
#include "Protocol/HallProtocol.h"
#include "Protocol/GameProtocol.h"

namespace
{
	FString NormalizeVendorItemId(FString InValue)
	{
		InValue.TrimStartAndEndInline();
		InValue = InValue.ToLower();

		FString Result;
		Result.Reserve(InValue.Len());
		for (const TCHAR Ch : InValue)
		{
			if (FChar::IsAlnum(Ch))
			{
				Result.AppendChar(Ch);
			}
		}
		return Result;
	}

	FString NormalizeVendorItemName(FString InValue)
	{
		InValue.TrimStartAndEndInline();
		InValue = InValue.ToLower();

		FString Result;
		Result.Reserve(InValue.Len());
		bool bLastWasSpace = false;
		for (const TCHAR Ch : InValue)
		{
			if (FChar::IsWhitespace(Ch))
			{
				if (!bLastWasSpace)
				{
					Result.AppendChar(TEXT(' '));
					bLastWasSpace = true;
				}
				continue;
			}

			Result.AppendChar(Ch);
			bLastWasSpace = false;
		}

		Result.TrimStartAndEndInline();
		return Result;
	}

	FString DescribeVendorItem(const FFS_ItemData* ItemData)
	{
		if (!ItemData)
		{
			return TEXT("<null>");
		}

		return FString::Printf(
			TEXT("Name:%s ID:%s Index:%d Type:%d Value:%.1f Quantity:%d Ptr:%p"),
			*ItemData->Description.Name.ToString(),
			*ItemData->Description.ID,
			ItemData->Index,
			static_cast<int32>(ItemData->Type),
			ItemData->Stats.Value,
			ItemData->Stacks.Quantity,
			ItemData);
	}
}

void UUI_VendorStorageInventory::UpdateInteraction(UInteractionComponent* ITCom)
{
	InteractionComponent = ITCom;
	ClearSelection();
	UE_LOG(MMOARPG, Display, TEXT("[VendorUI] UpdateInteraction [Interaction:%p DataTableType:%d]"),
		InteractionComponent,
		InteractionComponent ? static_cast<int32>(InteractionComponent->DataTableType) : INDEX_NONE);
	InitItems();
}

FReply UUI_VendorStorageInventory::NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (GetVisibility() == ESlateVisibility::Hidden ||
		GetVisibility() == ESlateVisibility::Collapsed)
	{
		return Super::NativeOnPreviewKeyDown(InGeometry, InKeyEvent);
	}

	if (AMMOARPGPlayerController* PC = GetOwningPlayer<AMMOARPGPlayerController>())
	{
		if (PC->TryHandleVendorKey(InKeyEvent.GetKey()))
		{
			return FReply::Handled();
		}
	}

	return Super::NativeOnPreviewKeyDown(InGeometry, InKeyEvent);
}

void UUI_VendorStorageInventory::FocusVendorPanel()
{
	if (APlayerController* PC = GetOwningPlayer())
	{
		UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(PC, this, EMouseLockMode::DoNotLock, true);
		PC->SetShowMouseCursor(true);
	}

	SetKeyboardFocus();
}

void UUI_VendorStorageInventory::NotifyPurchaseSucceeded(const FFS_ItemData* ItemData, int32 PurchasedCount)
{
	if (!ItemData || PurchasedCount <= 0 || !OwnedText)
	{
		return;
	}

	if (m_pItemData != ItemData)
	{
		return;
	}

	const int32 NewOwnedCount = GetOwnedItemCount(ItemData) + PurchasedCount;
	OwnedText->SetText(FText::FromString(FString::Printf(TEXT("Owned: %d"), NewOwnedCount)));
}

//#include <Components/VerticalBox.h>
//#include "UI_CraftingListSlot.h"

void UUI_VendorStorageInventory::NativeConstruct()
{
	Super::NativeConstruct();

	// 只注册 delegate，不切换上下文（widget 创建时可能是隐藏状态）
	if (AMMOARPGPlayerController* PC = GetOwningPlayer<AMMOARPGPlayerController>())
	{
		m_VendorHotkeyHandle = PC->OnVendorHotkey.AddUObject(this, &UUI_VendorStorageInventory::OnVendorHotkey);
	}

	if (UMMOARPGGameInstance* GI = GetGameInstance<UMMOARPGGameInstance>())
	{
		if (UMMOARPGNetSubsystem* Net = GI->GetSubsystem<UMMOARPGNetSubsystem>())
		{
			InterestingProtos = { SP_CharacterResponse, SP_ItemBuy };
			Net->AddProtoListenerBatch(InterestingProtos, this, &UUI_VendorStorageInventory::RecvProtocol, InterestingHandles);
		}
	}

	UpdateGoldText();
}

void UUI_VendorStorageInventory::NativeDestruct()
{
	if (UMMOARPGGameInstance* GI = GetGameInstance<UMMOARPGGameInstance>())
	{
		if (UMMOARPGNetSubsystem* Net = GI->GetSubsystem<UMMOARPGNetSubsystem>())
		{
			Net->RemoveProtoListenersBatch(InterestingProtos, InterestingHandles);
		}
	}

	InterestingProtos.Reset();
	InterestingHandles.Reset();

	if (AMMOARPGPlayerController* PC = GetOwningPlayer<AMMOARPGPlayerController>())
	{
		if (m_VendorHotkeyHandle.IsValid())
		{
			PC->OnVendorHotkey.Remove(m_VendorHotkeyHandle);
			m_VendorHotkeyHandle.Reset();
		}
	}

	Super::NativeDestruct();
}

bool UUI_VendorStorageInventory::TryCloseSplitStack()
{
	if (!m_SplitStack) return false;

	const ESlateVisibility V = m_SplitStack->GetVisibility();
	if (V == ESlateVisibility::Hidden || V == ESlateVisibility::Collapsed) return false;

	m_SplitStack->SetVisibility(ESlateVisibility::Hidden);
	UE_LOG(MMOARPG, Display, TEXT("[VendorUI] TryCloseSplitStack: closed SplitStack"));
	return true;
}

void UUI_VendorStorageInventory::OnVendorHotkey(ESystemHotkey Action)
{
	// [DBG] 临时日志 — 测试完删除
	UE_LOG(MMOARPG, Display, TEXT("[DBG-VendorStorage] OnVendorHotkey Action=%d SplitStack=%p"),
		static_cast<int32>(Action), m_SplitStack);

	// VendorOpenDialog（X）：打开购买数量弹窗
	if (Action == ESystemHotkey::VendorOpenDialog)
	{
		const bool bSplitOpen = m_SplitStack &&
			m_SplitStack->GetVisibility() != ESlateVisibility::Hidden &&
			m_SplitStack->GetVisibility() != ESlateVisibility::Collapsed;

		// [DBG] 临时日志 — 测试完删除
		UE_LOG(MMOARPG, Display, TEXT("[DBG-VendorStorage] VendorOpenDialog: bSplitOpen=%d SelectedItem=%p"),
			bSplitOpen, m_pItemData);

		if (!bSplitOpen)
		{
			OpenSplitStack();
		}
	}
}

void UUI_VendorStorageInventory::OpenSplitStack()
{
	if (!m_pItemData || !SplitStackClass) return;

	if (!m_SplitStack)
	{
		m_SplitStack = CreateWidget<UUI_SplitStack>(GetOwningPlayer(), SplitStackClass);
		if (m_SplitStack)
		{
			m_SplitStack->AddToViewport(10);
		}
	}

	if (m_SplitStack)
	{
		m_SplitStack->SetParents(this);
		m_SplitStack->InitWithItem(m_pItemData);
		m_SplitStack->SetVisibility(ESlateVisibility::Visible);
	}
}

void UUI_VendorStorageInventory::DeleteUpdateItem(const FFS_ItemData* ItemData)
{
	if (!ItemData)
	{
		ClearSelection();
		UE_LOG(MMOARPG, Warning, TEXT("[VendorUI] DeleteUpdateItem ignored because ItemData is null"));
		return;
	}

	if (m_pItemData != ItemData)
	{
		UUI_VendorStorageSlot** pOldWidget = mapSlot.Find(m_pItemData);
		if (pOldWidget && *pOldWidget)
		{
			(*pOldWidget)->OnUnFocus();
		}

		m_pItemData = ItemData;
		UUI_VendorStorageSlot** pNewWidget = mapSlot.Find(m_pItemData);
		if (pNewWidget && *pNewWidget)
		{
			(*pNewWidget)->OnFocus();
		}

		UE_LOG(MMOARPG, Display, TEXT("[VendorUI] Selected vendor item [%s]"), *DescribeVendorItem(m_pItemData));
	}

	UpdateMainToolTip(ItemData);
	UpdateOwnedText(ItemData);
	UpdateGoldText();
	UpdateItem(*ItemData);
}

void UUI_VendorStorageInventory::InitItems()
{
	if (!StorageUniformGrid)
	{
		ClearSelection();
		UE_LOG(MMOARPG, Warning, TEXT("[VendorUI] InitItems aborted because StorageUniformGrid is null"));
		return;
	}

	StorageUniformGrid->ClearChildren();
	AMMOARPGGameState* pGameState = GetGameState<AMMOARPGGameState>();
	if(!pGameState || !InteractionComponent)
	{
		ClearSelection();
		UE_LOG(MMOARPG, Warning, TEXT("[VendorUI] InitItems aborted [GameState:%p Interaction:%p]"),
			pGameState,
			InteractionComponent);
		return;
	}
	TArray<FFS_ItemData>* aItemList = pGameState->GetItemsByDataTableType(InteractionComponent->DataTableType);
	if(aItemList==NULL)
	{
		ClearSelection();
		UE_LOG(MMOARPG, Warning, TEXT("[VendorUI] InitItems failed to resolve item list [DataTableType:%d]"),
			static_cast<int32>(InteractionComponent->DataTableType));
		return;
	}

	UE_LOG(MMOARPG, Display, TEXT("[VendorUI] InitItems start [DataTableType:%d ItemCount:%d SlotClass:%s]"),
		static_cast<int32>(InteractionComponent->DataTableType),
		aItemList->Num(),
		*GetNameSafe(Slot_BPClass.Get()));

	m_pItemData = 0;
	int InRow = 0;
	mapSlot.Reset();
	for(const FFS_ItemData& ItemData : *aItemList)
	{
		{
			UUI_VendorStorageSlot* pWidget = CreateWidget<UUI_VendorStorageSlot>(GetWorld(), Slot_BPClass);
			if (!pWidget)
			{
				UE_LOG(MMOARPG, Warning, TEXT("[VendorUI] Failed to create vendor slot widget [Row:%d Item:%s]"),
					InRow,
					*DescribeVendorItem(&ItemData));
				continue;
			}

			pWidget->SetItemData(ItemData);
			mapSlot.Emplace(&ItemData, pWidget);
			//mapSlot.Add(&ItemData, pWidget);
			pWidget->FunUpdateItem.BindUObject(this, &ThisClass::DeleteUpdateItem);
			//pWidget->SetUpdateItemDelegate(FunUpdateItem);
			StorageUniformGrid->AddChildToUniformGrid(pWidget, InRow);
			UE_LOG(MMOARPG, Display, TEXT("[VendorUI] Added vendor slot [Row:%d %s]"),
				InRow,
				*DescribeVendorItem(&ItemData));
			InRow++;
			//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, TEXT("333"));
			//FText NameA, NameB;
			//if(ItemData->Description.Name.ToString() == "Helm of Fire King")
			 if(!m_pItemData)
			 {
				 m_pItemData = &ItemData;
				 UE_LOG(MMOARPG, Display, TEXT("[VendorUI] Default selected vendor item [%s]"),
					 *DescribeVendorItem(m_pItemData));
			 }
		}
	}
	UUI_VendorStorageSlot** pWidget = mapSlot.Find(m_pItemData);
	if(pWidget && *pWidget)
	{
		(*pWidget)->OnFocus();
		UE_LOG(MMOARPG, Display, TEXT("[VendorUI] Focus vendor slot and push UpdateItem [%s]"),
			*DescribeVendorItem(m_pItemData));
		UpdateMainToolTip(m_pItemData);
		UpdateOwnedText(m_pItemData);
		UpdateGoldText();
		UpdateItem(*m_pItemData);
	}
	else
	{
		UpdateMainToolTip(nullptr);
		UpdateOwnedText(nullptr);
		UpdateGoldText();
		UE_LOG(MMOARPG, Warning, TEXT("[VendorUI] No valid focused vendor slot after InitItems [Selected:%s MapSlotCount:%d]"),
			*DescribeVendorItem(m_pItemData),
			mapSlot.Num());
	}
}

void UUI_VendorStorageInventory::UpdateMainToolTip(const FFS_ItemData* ItemData) const
{
	if (WB_MainToolTip)
	{
		WB_MainToolTip->SetItemData(ItemData);
	}
}

void UUI_VendorStorageInventory::UpdateOwnedText(const FFS_ItemData* ItemData) const
{
	if (OwnedText)
	{
		OwnedText->SetText(FText::FromString(FString::Printf(TEXT("Owned: %d"), GetOwnedItemCount(ItemData))));
	}
}

void UUI_VendorStorageInventory::UpdateGoldText()
{
	if (!TextGold)
	{
		return;
	}

	UMMOARPGGameInstance* GI = GetGameInstance<UMMOARPGGameInstance>();
	const int32 GoldValue = GI ? static_cast<int32>(GI->GetUserData().base.econ.gold) : 0;
	TextGold->SetText(FText::AsNumber(GoldValue));
}

void UUI_VendorStorageInventory::RecvProtocol(uint32 ProtocolNumber)
{
	switch (ProtocolNumber)
	{
	case SP_CharacterResponse:
	case SP_ItemBuy:
		UpdateGoldText();
		break;
	default:
		break;
	}
}

void UUI_VendorStorageInventory::ClearSelection()
{
	if (UUI_VendorStorageSlot** pWidget = mapSlot.Find(m_pItemData))
	{
		if (*pWidget)
		{
			(*pWidget)->OnUnFocus();
		}
	}

	m_pItemData = nullptr;
	UpdateMainToolTip(nullptr);
	UpdateOwnedText(nullptr);
	UpdateGoldText();
}

int32 UUI_VendorStorageInventory::GetOwnedItemCount(const FFS_ItemData* ItemData) const
{
	if (!ItemData)
	{
		return 0;
	}

	APlayerController* OwningPC = GetOwningPlayer();
	APlayerController* PC = OwningPC;
	if (!PC)
	{
		if (UWorld* World = GetWorld())
		{
			PC = World->GetFirstPlayerController();
		}
	}

	const AMMOARPGPlayerState* PlayerState = PC ? PC->GetPlayerState<AMMOARPGPlayerState>() : nullptr;
	if (!PlayerState)
	{
		return 0;
	}

	const TArray<FFS_ItemData>& BagItems = PlayerState->GetBagItems();
	const FString TargetName = NormalizeVendorItemName(ItemData->Description.Name.ToString());
	const FString TargetId = NormalizeVendorItemId(ItemData->Description.ID);
	int32 OwnedCountByName = 0;
	int32 OwnedCount = 0;

	for (const FFS_ItemData& BagItem : BagItems)
	{
		const FString BagItemName = NormalizeVendorItemName(BagItem.Description.Name.ToString());
		if (!TargetName.IsEmpty() && BagItemName == TargetName)
		{
			OwnedCountByName += FMath::Max(1, BagItem.Stacks.Quantity);
			continue;
		}

		const bool bSameIndex = ItemData->Index > 0 && BagItem.Index == ItemData->Index;
		const bool bSameId = !TargetId.IsEmpty() && NormalizeVendorItemId(BagItem.Description.ID) == TargetId;
		if (!bSameIndex && !bSameId)
		{
			continue;
		}

		OwnedCount += FMath::Max(1, BagItem.Stacks.Quantity);
	}

	if (OwnedCountByName > 0 || !TargetName.IsEmpty())
	{
		return OwnedCountByName;
	}

	return OwnedCount;
}
