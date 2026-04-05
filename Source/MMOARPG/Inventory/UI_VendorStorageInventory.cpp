// Fill out your copyright notice in the Description page of Project Settings.

#include "UI_VendorStorageInventory.h"
#include "Components/UniformGridPanel.h"
#include "UI_VendorStorageSlot.h"
#include "../MMOARPG.h"
#include "../Core/Component/InventoryComponent.h"
#include "../Core/Component/InteractionComponent.h"
#include "../Core/Game/MMOARPGGameState.h"

namespace
{
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
	UE_LOG(MMOARPG, Display, TEXT("[VendorUI] UpdateInteraction [Interaction:%p DataTableType:%d]"),
		InteractionComponent,
		InteractionComponent ? static_cast<int32>(InteractionComponent->DataTableType) : INDEX_NONE);
	InitItems();
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
}

void UUI_VendorStorageInventory::NativeDestruct()
{
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

void UUI_VendorStorageInventory::OnVendorHotkey(ESystemHotkey Action)
{
	// [DBG] 临时日志 — 测试完删除
	UE_LOG(MMOARPG, Display, TEXT("[DBG-VendorStorage] OnVendorHotkey Action=%d SplitStack=%p"),
		static_cast<int32>(Action), m_SplitStack);

	// SplitStack 可见时不处理 VendorBuy（让 SplitStack 自己响应确认购买）
	if (Action == ESystemHotkey::VendorBuy)
	{
		const bool bSplitOpen = m_SplitStack &&
			m_SplitStack->GetVisibility() != ESlateVisibility::Hidden &&
			m_SplitStack->GetVisibility() != ESlateVisibility::Collapsed;

		// [DBG] 临时日志 — 测试完删除
		UE_LOG(MMOARPG, Display, TEXT("[DBG-VendorStorage] VendorBuy: bSplitOpen=%d SelectedItem=%p"),
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
		m_SplitStack->InitWithItem(m_pItemData);
		m_SplitStack->SetVisibility(ESlateVisibility::Visible);
	}
}

void UUI_VendorStorageInventory::DeleteUpdateItem(const FFS_ItemData* ItemData)
{
	if (!ItemData)
	{
		UE_LOG(MMOARPG, Warning, TEXT("[VendorUI] DeleteUpdateItem ignored because ItemData is null"));
		return;
	}

	if(m_pItemData == ItemData)
	{
		UE_LOG(MMOARPG, Display, TEXT("[VendorUI] DeleteUpdateItem ignored because selection is unchanged [%s]"),
			*DescribeVendorItem(ItemData));
		return;
	}
	UUI_VendorStorageSlot** pWidget = mapSlot.Find(m_pItemData);
	if(pWidget && *pWidget)
	{
		(*pWidget)->OnUnFocus();
	}
	m_pItemData = ItemData;
	UE_LOG(MMOARPG, Display, TEXT("[VendorUI] Selected vendor item [%s]"), *DescribeVendorItem(m_pItemData));
	UpdateItem(*ItemData);
}

void UUI_VendorStorageInventory::InitItems()
{
	if (!StorageUniformGrid)
	{
		UE_LOG(MMOARPG, Warning, TEXT("[VendorUI] InitItems aborted because StorageUniformGrid is null"));
		return;
	}

	StorageUniformGrid->ClearChildren();
	AMMOARPGGameState* pGameState = GetGameState<AMMOARPGGameState>();
	if(!pGameState || !InteractionComponent)
	{
		UE_LOG(MMOARPG, Warning, TEXT("[VendorUI] InitItems aborted [GameState:%p Interaction:%p]"),
			pGameState,
			InteractionComponent);
		return;
	}
	TArray<FFS_ItemData>* aItemList = pGameState->GetItemsByDataTableType(InteractionComponent->DataTableType);
	if(aItemList==NULL)
	{
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
		UpdateItem(*m_pItemData);
	}
	else
	{
		UE_LOG(MMOARPG, Warning, TEXT("[VendorUI] No valid focused vendor slot after InitItems [Selected:%s MapSlotCount:%d]"),
			*DescribeVendorItem(m_pItemData),
			mapSlot.Num());
	}
}
