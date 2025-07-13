#pragma once

#include "CoreMinimal.h"
#include "UI_Base.h"

#include "UI_CraftingItem.generated.h"

class UButton;
class UImage;
struct FFS_ItemData;
class UTextBlock;
//class UUserWidget;

UCLASS()
class UUI_CraftingItem : public UUI_Base
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	UImage* Image_Item;

	UPROPERTY(meta = (BindWidget))
	UImage* Anim_img;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextQuantity;
public:
	virtual void NativeConstruct();

	virtual void NativeDestruct();

	void UpdateItem(const FFS_ItemData* pItemData);
//
//private:
//	FFS_ItemData* m_ItemData = NULL;
};
