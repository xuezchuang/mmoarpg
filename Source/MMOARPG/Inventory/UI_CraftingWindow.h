#pragma once

#include "CoreMinimal.h"
#include "UI_Base.h"

#include "UI_CraftingWindow.generated.h"

class UButton;
class UImage;
struct FFS_ItemData;
class UUI_CraftingList;
class UUI_CraftingItem;
class UUI_CraftingRequiredItemsBar;
class UTextBlock;
class UHorizontalBox;
class UProgressBar;
//class UUserWidget;

UCLASS()
class UUI_CraftingWindow: public UUI_Base
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	UUI_CraftingList* WB_CraftingList;

	UPROPERTY(meta = (BindWidget))
	UUI_CraftingItem* WB_CraftingItem;
	
	UPROPERTY(meta = (BindWidget))
	UUI_CraftingRequiredItemsBar* WB_CraftingRequiredItemsBar;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextDesName;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextRatity;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextDescription;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextLevel;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* WEIGHT;

	UPROPERTY(meta = (BindWidget))
	UImage* Weight_Image;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextWeight;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Weight_Quantity_text;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* COST;

	UPROPERTY(meta = (BindWidget))
	UImage* Coins_Image;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextValue;

	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* Box_StackedWeight;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextDurability;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* ProgressBar;

	UPROPERTY(meta = (BindWidget))
	UButton* Button_CRAFT;
public:
	virtual void NativeConstruct();

	virtual void NativeDestruct();

//
private:
//	FFS_ItemData* m_ItemData = NULL;
	void UpdateItem(const FFS_ItemData* ItemData);

	UFUNCTION()
	void OnCraft();
};
