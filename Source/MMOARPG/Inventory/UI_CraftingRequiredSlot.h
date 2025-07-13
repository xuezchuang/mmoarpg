#pragma once

#include "CoreMinimal.h"
#include "UI_Base.h"
#include "FItemData.h"
#include "UI_CraftingRequiredSlot.generated.h"

class UButton;
class UTextBlock;
class UImage;
//class UUserWidget;

UCLASS()
class UUI_CraftingRequiredSlot : public UUI_Base
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	UImage* Image_Item;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Quantity_current;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Quantity_need;
public:
	virtual void NativeConstruct();

	virtual void NativeDestruct();

	void SetItemData(const FFS_ItemData* pItemData);
private:
	UFUNCTION()
	void OnClick();

	const FFS_ItemData* m_ItemData = NULL;

};
