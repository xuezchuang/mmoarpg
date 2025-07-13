#pragma once

#include "CoreMinimal.h"
#include "UI_Base.h"
#include "UI_CraftingRequiredItemsBar.generated.h"

struct FFS_ItemData;
class UVerticalBox;
class UUI_CraftingRequiredSlot;

UCLASS()
class UUI_CraftingRequiredItemsBar : public UUI_Base
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	UVerticalBox* VerticalBox;


	UPROPERTY()
	TSubclassOf<UUI_CraftingRequiredSlot> BP_CraftingRequiredSlot;

public:
	UUI_CraftingRequiredItemsBar();
	virtual void NativeConstruct();

	virtual void NativeDestruct();

	void UpdateItem(const FFS_ItemData* pItemData);
//
//private:
//	FFS_ItemData* m_ItemData = NULL;
};
