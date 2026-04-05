// Fill out your copyright notice in the Description page of Project Settings.

#include "UI_InventoryBase.h"
#include "Components/Image.h"
#include "Components/UniformGridPanel.h"
#include "Blueprint/UserWidget.h"
#include "../Core/Game/MMOARPGPlayerState.h"
#include "GameFramework/PlayerState.h"

void UUI_InventoryBase::NativeConstruct()
{
	Super::NativeConstruct();
}

void UUI_InventoryBase::NativeDestruct()
{
	Super::NativeDestruct();
}

void UUI_InventoryBase::NativePreConstruct()
{
	Super::NativePreConstruct();
}

UUniformGridPanel* UUI_InventoryBase::GetGridByCategory(E_InventoryCategory Category) const
{
	switch (Category)
	{
	case E_InventoryCategory::Weapon:      return UniformGrid_P1;
	case E_InventoryCategory::Range:       return UniformGrid_P2;
	case E_InventoryCategory::Armor:       return UniformGrid_P3;
	case E_InventoryCategory::Accessories: return UniformGrid_P4;
	case E_InventoryCategory::RuneStone:   return UniformGrid_P5;
	case E_InventoryCategory::Consumable:  return UniformGrid_P6;
	case E_InventoryCategory::Resources:   return UniformGrid_P7;
	case E_InventoryCategory::Mount:       return UniformGrid_P8;
	case E_InventoryCategory::Quest:       return UniformGrid_P9;
	case E_InventoryCategory::Event:       return UniformGrid_P10;
	default:                               return nullptr;
	}
}

void UUI_InventoryBase::PopulateGridForCategory(E_InventoryCategory Category)
{
	UUniformGridPanel* Grid = GetGridByCategory(Category);
	if (!Grid || !InventorySlotClass)
	{
		return;
	}

	Grid->ClearChildren();

	// 从 PlayerState 获取该分类的物品
	APlayerController* PC = GetOwningPlayer();
	if (!PC) return;

	AMMOARPGPlayerState* PS = PC->GetPlayerState<AMMOARPGPlayerState>();
	if (!PS) return;

	TArray<FFS_ItemData*> Items = PS->GetBagItemsByCategory(Category);
	int32 Row = 0;
	for (FFS_ItemData* Item : Items)
	{
		UUserWidget* SlotWidget = CreateWidget<UUserWidget>(this, InventorySlotClass);
		if (SlotWidget)
		{
			Grid->AddChildToUniformGrid(SlotWidget, Row, 0);
			++Row;
		}
	}
}
