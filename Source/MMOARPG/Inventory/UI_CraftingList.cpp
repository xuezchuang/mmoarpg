// Fill out your copyright notice in the Description page of Project Settings.

#include "UI_CraftingList.h"
#include "MMOARPG.h"
#include "UMG/Public/Components/WidgetSwitcher.h"
#include "UMG/Public/Components/VerticalBox.h"
#include "UMG/Public/Components/TextBlock.h"
#include "UMG/Public/Components/Border.h"
#include "UMG/Public/Components/Button.h"
#include "InventoryGameState.h"
#include "UI_CraftingListSlot.h"
#include "UI_CraftingListSpacer.h"



UUI_CraftingList::UUI_CraftingList()
{
	static ConstructorHelpers::FClassFinder<UUI_CraftingListSlot> tBP_CraftingListSlot(TEXT("/Game/MetanoiaCombat/Inventory/UI/Crafting/WB_CraftingListSlot"));
	BP_CraftingListSlot = tBP_CraftingListSlot.Class;
}

void UUI_CraftingList::NativeConstruct()
{
	Super::NativeConstruct(); 
	Button_Armor->OnClicked.AddDynamic(this, &UUI_CraftingList::OnButtonArmor);
	Button_Weapons->OnClicked.AddDynamic(this, &UUI_CraftingList::OnButtonWeapons);
	Button_Bows->OnClicked.AddDynamic(this, &UUI_CraftingList::OnButtonBows);
	Button_Shield->OnClicked.AddDynamic(this, &UUI_CraftingList::OnButtonShield);
	Button_Accessories->OnClicked.AddDynamic(this, &UUI_CraftingList::OnButtonAccessories);
	const FLinearColor BGColor = FLinearColor(0.0f, 0.0f, 0.0f, 0.0f);
	GetBorder(E_ItemType::Armor)->SetBrushColor(BGColor);
	GetBorder(E_ItemType::Weapon)->SetBrushColor(BGColor);
	GetBorder(E_ItemType::Bow)->SetBrushColor(BGColor);
	GetBorder(E_ItemType::Shield)->SetBrushColor(BGColor);
	GetBorder(E_ItemType::Accessories)->SetBrushColor(BGColor);

}

void UUI_CraftingList::NativeDestruct()
{
	Super::NativeDestruct();
}

void UUI_CraftingList::Init()
{
	AInventoryGameState* pGameState = GetGameState<AInventoryGameState>();
	//if (pGameState)
	//	pGameState->GetCraftingItemListByType(aItemList);
	//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, FString::Printf(TEXT("%d"), aItemList.Num()));
	FFS_ItemData* InitItemData = 0;
	for (FFS_ItemData* ItemData : aItemList)
	{
		UVerticalBox* pVerticalBox = Cast<UVerticalBox>(GetSpacer(ItemData->Slot, ItemData->WeaponCategory)->GetBox());
		if (pVerticalBox)
		{
			UUI_CraftingListSlot* pWidget = CreateWidget<UUI_CraftingListSlot>(GetWorld(), BP_CraftingListSlot);
			pWidget->SetItemData(ItemData);
			pWidget->SetUpdateItemDelegate(FunUpdateItem);
			pVerticalBox->AddChildToVerticalBox(pWidget);
			//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, TEXT("333"));
			FText NameA,NameB;
			if (ItemData->Description.Name.ToString() == "Helm of Fire King")
				InitItemData = ItemData;
		}
	}
	//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, FString::Printf(TEXT("%d"), aItemList.Num()));
	if (InitItemData)
		FunUpdateItem.Execute(InitItemData);
	SwitchListType(E_ItemType::Armor);
}

void UUI_CraftingList::OnClicked()
{
}

void UUI_CraftingList::UnClicked()
{
}

void UUI_CraftingList::OnButtonArmor()
{
	SwitchListType(E_ItemType::Armor);
}
void UUI_CraftingList::OnButtonWeapons()
{
	SwitchListType(E_ItemType::Weapon);
}
void UUI_CraftingList::OnButtonBows()
{
	SwitchListType(E_ItemType::Bow);
}
void UUI_CraftingList::OnButtonShield()
{
	SwitchListType(E_ItemType::Shield);
}
void UUI_CraftingList::OnButtonAccessories()
{
	SwitchListType(E_ItemType::Accessories);
}

void UUI_CraftingList::SwitchListType(E_ItemType eItemType)
{
	if (m_CurItemType == eItemType)
		return;
	FString EnumName = StaticEnum<E_ItemType>()->GetNameStringByValue(static_cast<int32>(eItemType));
	Title->SetText(FText::FromString(EnumName)); 
	SetBorder(eItemType); 
	WidgetSwitcher->SetActiveWidget(GetVerBox(eItemType));
	m_CurItemType = eItemType;
}

UVerticalBox* UUI_CraftingList::GetVerBox(E_ItemType eItemType)
{
	UVerticalBox* pVerBox = NULL;
	switch (eItemType)
	{
	case E_ItemType::None:
		break;
	case E_ItemType::Armor:
		pVerBox = MainVBox_Armor;
		break;
	case E_ItemType::Weapon:
		pVerBox = MainVBox_Weapons;
		break;
	case E_ItemType::Shield:
		pVerBox = MainVBox_Shield;
		break;
	case E_ItemType::Food:
		break;
	case E_ItemType::Potion:
		break;
	case E_ItemType::Crafting_Ingredient:
		break;
	case E_ItemType::Quest_Item:
		break;
	case E_ItemType::Other:
		break;
	case E_ItemType::Currency:
		break;
	case E_ItemType::Bow:
		pVerBox = MainVBox_Range;
		break;
	case E_ItemType::Arrow:
		break;
	case E_ItemType::Accessories:
		pVerBox = MainVBox_Accessories;
		break;
	case E_ItemType::Horse_Saddle:
		break;
	case E_ItemType::Horse_Armor:
		break;
	case E_ItemType::Horse_Reins:
		break;
	case E_ItemType::Horse_Wings:
		break;
	case E_ItemType::Horse_Horn:
		break;
	case E_ItemType::Glider:
		break;
	case E_ItemType::Mining_Tool:
		break;
	case E_ItemType::Logging_Tool:
		break;
	case E_ItemType::Fishing_Tool:
		break;
	default:
		break;
	}
	return pVerBox;
}

void UUI_CraftingList::SetBorder(E_ItemType eItemType)
{
	if (m_CurItemType == eItemType)
		return;
	UBorder* pBorder = GetBorder(m_CurItemType);
	if (pBorder)
		pBorder->SetBrushColor(FLinearColor(0.0f, 0.0f, 0.0f, 0.0f));
	pBorder = GetBorder(eItemType);
	if (pBorder)
		pBorder->SetBrushColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
}

UBorder* UUI_CraftingList::GetBorder(E_ItemType eItemType)
{
	UBorder* pBorder = NULL;
	switch (eItemType)
	{
	default:
		break;
	case E_ItemType::None:
		break;
	case E_ItemType::Armor:
		pBorder = Border_Armor;
		break;
	case E_ItemType::Weapon:
		pBorder = Border_Weapons;
		break;
	case E_ItemType::Shield:
		pBorder = Border_Shield;
		break;
	case E_ItemType::Food:
		break;
	case E_ItemType::Potion:
		break;
	case E_ItemType::Crafting_Ingredient:
		break;
	case E_ItemType::Quest_Item:
		break;
	case E_ItemType::Other:
		break;
	case E_ItemType::Currency:
		break;
	case E_ItemType::Bow:
		pBorder = Border_Bows;
		break;
	case E_ItemType::Arrow:
		break;
	case E_ItemType::Accessories:
		pBorder = Border_Accessories;
		break;
	case E_ItemType::Horse_Saddle:
		break;
	case E_ItemType::Horse_Armor:
		break;
	case E_ItemType::Horse_Reins:
		break;
	case E_ItemType::Horse_Wings:
		break;
	case E_ItemType::Horse_Horn:
		break;
	case E_ItemType::Glider:
		break;
	case E_ItemType::Mining_Tool:
		break;
	case E_ItemType::Logging_Tool:
		break;
	case E_ItemType::Fishing_Tool:
		break;
	}
	return pBorder;
}

UUI_CraftingListSpacer* UUI_CraftingList::GetSpacer(E_ItemSlot eItemSlot,E_WeaponCategory eWeaponCategory)
{
	UUI_CraftingListSpacer* pSpacer = NULL;
	switch (eItemSlot)
	{
	default:
		break;
	case E_ItemSlot::None:
		break;
	case E_ItemSlot::Head:
		pSpacer = Spacer_Armor_Head;
		break;
	case E_ItemSlot::Gloves:
		pSpacer = Spacer_Armor_Gloves;
		break;
	case E_ItemSlot::Neck:
		pSpacer = Spacer_Necklace;
		break;
	case E_ItemSlot::Chestplate:
		pSpacer = Spacer_Armor_Chest;
		break;
	case E_ItemSlot::Legs:
		pSpacer = Spacer_Armor_Legs;
		break;
	case E_ItemSlot::Boots:
		pSpacer = Spacer_Armor_Boots;
		break;
	case E_ItemSlot::Weapon:
		switch (eWeaponCategory)
		{
		default:
			break;
		case E_WeaponCategory::None:
			break;
		case E_WeaponCategory::Sword:
			pSpacer = Spacer_Swords;
			break;
		case E_WeaponCategory::Warhammer:
			pSpacer = Spacer_Hammers;
			break;
		case E_WeaponCategory::Axe:
			pSpacer = Spacer_Axes;
			break;
		case E_WeaponCategory::Mace:
			break;
		case E_WeaponCategory::Bow:
			break;
		case E_WeaponCategory::Arrow:
			break;
		case E_WeaponCategory::Greatsword:
			pSpacer = Spacer_GreatSword;
			break;
		case E_WeaponCategory::SwordShield:
			break;
		case E_WeaponCategory::DualWield:
			pSpacer = Spacer_DualSwords;
			break;
		case E_WeaponCategory::Spear:
			break;
		case E_WeaponCategory::Blunt:
			break;
		case E_WeaponCategory::MiningAxe:
			break;
		case E_WeaponCategory::LoggingAxe:
			break;
		case E_WeaponCategory::FishingRod:
			break;
		case E_WeaponCategory::Katana:
			break;
		}
		break;
	case E_ItemSlot::Shield:
		pSpacer = Spacer_Shield;
		break;
	case E_ItemSlot::Pocket1:
		break;
	case E_ItemSlot::Pocket2:
		break;
	case E_ItemSlot::Pocket3:
		break;
	case E_ItemSlot::Pocket4:
		break;
	case E_ItemSlot::Bow:
		pSpacer = Spacer_Bow;
		break;
	case E_ItemSlot::Arrow:
		pSpacer = Spacer_Arrow;
		break;
	case E_ItemSlot::Talisman:
		pSpacer = Spacer_Talisman;
		break;
	case E_ItemSlot::Ring:
		pSpacer = Spacer_Ring;
		break;
	case E_ItemSlot::Bracelet:
		pSpacer = Spacer_Bracelet;
		break;
	case E_ItemSlot::Saddle:
		break;
	case E_ItemSlot::Barding:
		break;
	case E_ItemSlot::Reins:
		break;
	case E_ItemSlot::Mount:
		break;
	case E_ItemSlot::HHorn:
		break;
	case E_ItemSlot::Glider:
		break;
	case E_ItemSlot::Mining:
		break;
	case E_ItemSlot::Logging:
		break;
	case E_ItemSlot::Fishing:
		break;
	}
	return pSpacer;
}
