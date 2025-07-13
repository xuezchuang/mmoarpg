#pragma once

#include "CoreMinimal.h"
#include "UI_Base.h"
#include "Engine\Texture2D.h"

#include "UI_InventoryBase.generated.h"

class UButton;
class UImage;
class UUI_CategoryButton;
//class UUserWidget;

UCLASS()
class UUI_InventoryBase : public UUI_Base
{
	GENERATED_BODY()
#pragma region UI_CategoryButton

	UPROPERTY(meta = (BindWidget))
	UUI_CategoryButton* WB_CategoryWeapon;

	UPROPERTY(meta = (BindWidget))
	UUI_CategoryButton* WB_CategoryRange;

	UPROPERTY(meta = (BindWidget))
	UUI_CategoryButton* WB_CategoryArmor;

	UPROPERTY(meta = (BindWidget))
	UUI_CategoryButton* WB_CategoryAccessories;

	UPROPERTY(meta = (BindWidget))
	UUI_CategoryButton* WB_CategoryRuneStone;

	UPROPERTY(meta = (BindWidget))
	UUI_CategoryButton* WB_CategoryConsumable;

	UPROPERTY(meta = (BindWidget))
	UUI_CategoryButton* WB_CategoryResources;

	UPROPERTY(meta = (BindWidget))
	UUI_CategoryButton* WB_CategoryMount;

	UPROPERTY(meta = (BindWidget))
	UUI_CategoryButton* WB_CategoryQuest;

	UPROPERTY(meta = (BindWidget))
	UUI_CategoryButton* WB_CategoryEvent;
#pragma endregion


public:
	virtual void NativeConstruct();

	virtual void NativeDestruct();

protected:
	void NativePreConstruct() override;

};
