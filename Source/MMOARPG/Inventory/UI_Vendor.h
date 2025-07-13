#pragma once

#include "CoreMinimal.h"
#include "UI_MainBase.h"
#include "Engine\Texture2D.h"

#include "UI_Vendor.generated.h"

class UButton;
class UImage;
class UUI_InventoryBase;
class UUI_VendorStorageInventory;
class UUI_UpperUIBar;
class UWidgetSwitcher;
class UInteractionComponent;
//class UUserWidget;

UCLASS()
class UUI_Vendor : public UUI_MainBase
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, Category = "BP_Setting")
	UInteractionComponent* InteractionComponent = NULL;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UUI_VendorStorageInventory* WB_VendorStorageInventory;

	//UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	//UUI_UpperUIBar* WB_UpperUIBar;

	//UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	//UWidgetSwitcher* WidgetSwitcher;

	//UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	//void ShowVendor();
//protected:
public:
	void VendorBuy(uint16 count, uint32 propid);
	void VendorSell(uint8 pos, uint16 count, uint32 propid);

	//UFUNCTION(BlueprintCallable, Category = "Widget")
	virtual void SetVisibility(ESlateVisibility InVisibility);
public:
	virtual void NativeConstruct();

	virtual void NativeDestruct();

protected:
	void NativePreConstruct() override;


	virtual void RecvProtocol(uint32 ProtocolNumber, FSimpleChannel* Channel) override;
	
private:
	//UFUNCTION()
	//void SetNativeUIType(E_UIType eType);


};
