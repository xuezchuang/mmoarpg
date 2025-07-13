#pragma once

#include "CoreMinimal.h"
#include "UI_Base.h"
#include "../Data/FItemData.h"
#include "UI_VendorStorageInventory.generated.h"

class UUI_CategoryButton;
class UUniformGridPanel;
class UUI_VendorStorageSlot;
class UInteractionComponent;

UCLASS()
class UUI_VendorStorageInventory : public UUI_Base
{
	GENERATED_BODY()

	//UPROPERTY(meta = (BindWidget))
	//UUI_CategoryButton* WB_VendorBuy;

	//UPROPERTY(meta = (BindWidget))
	//UUI_CategoryButton* WB_VendorSellBack;

public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UUniformGridPanel* StorageUniformGrid;

	UPROPERTY(EditDefaultsOnly, Category = "BP_Setting")
	TSubclassOf<UUI_VendorStorageSlot> Slot_BPClass;

	UPROPERTY(BlueprintReadWrite, Category = "BP_Setting")
	UInteractionComponent* InteractionComponent = NULL;
	//protected:
public:
	void UpdateInteraction(UInteractionComponent* ITCom);

	UFUNCTION(BlueprintImplementableEvent, Category = "BP_Setting")
	void UpdateItem(const FFS_ItemData& ItemData);

	//UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	//void UnOnFocus();
public:
	virtual void NativeConstruct();

	virtual void NativeDestruct();
private:

	void DeleteUpdateItem(const FFS_ItemData* ItemData);

	void InitItems();

	const FFS_ItemData* m_pItemData = NULL;
	TMap<const FFS_ItemData*, UUI_VendorStorageSlot*> mapSlot;
};
