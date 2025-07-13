#pragma once

#include "CoreMinimal.h"
#include "UI_Base.h"
#include "FItemData.h"
#include "UI_CraftingListSlot.generated.h"

class UButton;
class UTextBlock;
class UImage;
//class UUserWidget;

UCLASS()
class UUI_CraftingListSlot: public UUI_Base
{
	GENERATED_BODY()
//protected:

	UPROPERTY(meta = (BindWidget))
	UButton* BG_Button;

	UPROPERTY(meta = (BindWidget))
	UButton* Item_Image;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Item_rarity;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Item_name;
	
	UPROPERTY(meta = (BindWidget))
	UImage* LineColorRarity;

	//
	UPROPERTY(EditDefaultsOnly, Category = "UISetting")
	FSlateColor Common;

	UPROPERTY(EditDefaultsOnly, Category = "UISetting")
	FSlateColor Superior;

	UPROPERTY(EditDefaultsOnly, Category = "UISetting")
	FSlateColor Epic;

	UPROPERTY(EditDefaultsOnly, Category = "UISetting")
	FSlateColor Legendary;

public:
	virtual void NativeConstruct();
	
	virtual void NativeDestruct();

	void SetItemData(FFS_ItemData* pItemData);
	void SetUpdateItemDelegate(FUpdateItem _FunDelegate);
private:
	UFUNCTION()
	void OnClick();

	FFS_ItemData* m_ItemData = NULL;

	FUpdateItem FunUpdateItem;
};
