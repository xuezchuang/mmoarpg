// Fill out your copyright notice in the Description page of Project Settings.

#include "UI_VendorStorageInventory.h"
#include "Components/UniformGridPanel.h"
#include "InventoryGameState.h"
#include "UI_VendorStorageSlot.h"
#include "../Core/Component/InventoryComponent.h"
#include "../Core/Component/InteractionComponent.h"

void UUI_VendorStorageInventory::UpdateInteraction(UInteractionComponent* ITCom)
{
	InteractionComponent = ITCom;
	InitItems();
}

//#include <Components/VerticalBox.h>
//#include "UI_CraftingListSlot.h"

void UUI_VendorStorageInventory::NativeConstruct()
{
	Super::NativeConstruct();
}

void UUI_VendorStorageInventory::NativeDestruct()
{
	Super::NativeDestruct();
}

void UUI_VendorStorageInventory::DeleteUpdateItem(const FFS_ItemData* ItemData)
{
	if(m_pItemData == ItemData)
		return;
	UUI_VendorStorageSlot** pWidget = mapSlot.Find(m_pItemData);
	if(pWidget && *pWidget)
	{
		(*pWidget)->OnUnFocus();
	}
	m_pItemData = ItemData;
	UpdateItem(*ItemData);
}

void UUI_VendorStorageInventory::InitItems()
{
	StorageUniformGrid->ClearChildren();
	AInventoryGameState* pGameState = GetGameState<AInventoryGameState>();
	if(!pGameState || !InteractionComponent)
	{
		return;
	}
	TArray<FFS_ItemData>* aItemList = pGameState->GetItemsByDataTableType(InteractionComponent->DataTableType);
	if(aItemList==NULL)
	{
		return;
	}

	m_pItemData = 0;
	int InRow = 0;
	mapSlot.Reset();
	for(const FFS_ItemData& ItemData : *aItemList)
	{
		{
			UUI_VendorStorageSlot* pWidget = CreateWidget<UUI_VendorStorageSlot>(GetWorld(), Slot_BPClass);
			pWidget->SetItemData(ItemData);
			mapSlot.Emplace(&ItemData, pWidget);
			//mapSlot.Add(&ItemData, pWidget);
			pWidget->FunUpdateItem.BindUObject(this, &ThisClass::DeleteUpdateItem);
			//pWidget->SetUpdateItemDelegate(FunUpdateItem);
			StorageUniformGrid->AddChildToUniformGrid(pWidget, InRow);
			InRow++;
			//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, TEXT("333"));
			//FText NameA, NameB;
			//if(ItemData->Description.Name.ToString() == "Helm of Fire King")
			 if(!m_pItemData)
			 {
				 m_pItemData = &ItemData;
			 }
		}
	}
	UUI_VendorStorageSlot** pWidget = mapSlot.Find(m_pItemData);
	if(pWidget && *pWidget)
	{
		(*pWidget)->OnFocus();
		UpdateItem(*m_pItemData);
	}
}
