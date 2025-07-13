#pragma once

#include "CoreMinimal.h"
#include "UI_Base.h"
#include "../Data/FItemData.h"
#include "UI_CraftingList.generated.h"

class UButton;
class UUI_CraftingListSpacer;
class UBorder;
class UWidgetSwitcher;
class UVerticalBox;
class UTextBlock;
class UUI_CraftingListSlot;

UCLASS()
class UUI_CraftingList: public UUI_Base
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	UButton* Button_Armor;

	UPROPERTY(meta = (BindWidget))
	UButton* Button_Weapons;

	UPROPERTY(meta = (BindWidget))
	UButton* Button_Bows;

	UPROPERTY(meta = (BindWidget))
	UButton* Button_Shield;

	UPROPERTY(meta = (BindWidget))
	UButton* Button_Accessories;
	//Armor
	UPROPERTY(meta = (BindWidget))
	UUI_CraftingListSpacer* Spacer_Armor_Head;

	UPROPERTY(meta = (BindWidget))
	UUI_CraftingListSpacer* Spacer_Armor_Gloves;

	UPROPERTY(meta = (BindWidget))
	UUI_CraftingListSpacer* Spacer_Armor_Chest;

	UPROPERTY(meta = (BindWidget))
	UUI_CraftingListSpacer* Spacer_Armor_Legs;

	UPROPERTY(meta = (BindWidget))
	UUI_CraftingListSpacer* Spacer_Armor_Boots;
	
	//Weapons
	UPROPERTY(meta = (BindWidget))
	UUI_CraftingListSpacer* Spacer_Swords;

	UPROPERTY(meta = (BindWidget))
	UUI_CraftingListSpacer* Spacer_Hammers;

	UPROPERTY(meta = (BindWidget))
	UUI_CraftingListSpacer* Spacer_Axes;

	UPROPERTY(meta = (BindWidget))
	UUI_CraftingListSpacer* Spacer_GreatSword;

	UPROPERTY(meta = (BindWidget))
	UUI_CraftingListSpacer* Spacer_DualSwords;

	//Range
	UPROPERTY(meta = (BindWidget))
	UUI_CraftingListSpacer* Spacer_Bow;

	UPROPERTY(meta = (BindWidget))
	UUI_CraftingListSpacer* Spacer_Arrow;

	//Shield
	UPROPERTY(meta = (BindWidget))
	UUI_CraftingListSpacer* Spacer_Shield;

	//Accessories
	UPROPERTY(meta = (BindWidget))
	UUI_CraftingListSpacer* Spacer_Necklace;

	UPROPERTY(meta = (BindWidget))
	UUI_CraftingListSpacer* Spacer_Talisman;

	UPROPERTY(meta = (BindWidget))
	UUI_CraftingListSpacer* Spacer_Ring;

	UPROPERTY(meta = (BindWidget))
	UUI_CraftingListSpacer* Spacer_Bracelet;

	//
	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher* WidgetSwitcher;

	UPROPERTY(meta = (BindWidget))
	UVerticalBox* MainVBox_Armor;

	UPROPERTY(meta = (BindWidget))
	UVerticalBox* MainVBox_Weapons;

	UPROPERTY(meta = (BindWidget))
	UVerticalBox* MainVBox_Range;

	UPROPERTY(meta = (BindWidget))
	UVerticalBox* MainVBox_Shield;

	UPROPERTY(meta = (BindWidget))
	UVerticalBox* MainVBox_Accessories;

	//
	UPROPERTY(meta = (BindWidget))
	UBorder* Border_Armor;
	UPROPERTY(meta = (BindWidget))
	UBorder* Border_Weapons;
	UPROPERTY(meta = (BindWidget))
	UBorder* Border_Bows;
	UPROPERTY(meta = (BindWidget))
	UBorder* Border_Shield;
	UPROPERTY(meta = (BindWidget))
	UBorder* Border_Accessories;
	//
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Title;

	UPROPERTY(EditInstanceOnly, Category = "UISetting")
	FText TextBlock_Title;

	UPROPERTY()
	TSubclassOf<UUI_CraftingListSlot> BP_CraftingListSlot;

public:
	UUI_CraftingList();

	virtual void NativeConstruct();

	virtual void NativeDestruct();

	void Init();
	void OnClicked();
	void UnClicked();

	FUpdateItem FunUpdateItem;
private:
	UFUNCTION()
	void OnButtonArmor();

	UFUNCTION()
	void OnButtonWeapons();

	UFUNCTION()
	void OnButtonBows();

	UFUNCTION()
	void OnButtonShield();

	UFUNCTION()
	void OnButtonAccessories();

	void SwitchListType(E_ItemType eItemType);
	UVerticalBox* GetVerBox(E_ItemType eItemType);
	void SetBorder(E_ItemType eItemType);
	UBorder* GetBorder(E_ItemType eItemType);
	UUI_CraftingListSpacer* GetSpacer(E_ItemSlot eItemSlot,E_WeaponCategory eWeaponCategory);
private:
	E_ItemType m_CurItemType = E_ItemType::None;
	TArray<FFS_ItemData*> aItemList;
	TArray<FFS_ItemData*> aIngredientItemList;
};
