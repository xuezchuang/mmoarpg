#pragma once

#include "CoreMinimal.h"
#include "UI_Base.h"
#include "../Data/FItemData.h"
#include "UI_VendorStorageSlot.generated.h"

class UButton;
class UTextBlock;
class UImage;

UCLASS()
class UUI_VendorStorageSlot : public UUI_Base
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* Quantity;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* NameText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* ValueText;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UImage* ItemIcon;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UImage* BG;

	UPROPERTY(BlueprintReadWrite,meta = (BindWidget))
	UTextBlock* PurchaseMax;
	
	//protected:
public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* Native_Button;
public:
	virtual void NativeConstruct();

	virtual void NativeDestruct();

	void SetItemData(const FFS_ItemData& pItemData);

	FUpdateItem FunUpdateItem;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = UI)
	void OnFocus();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = UI)
	void OnUnFocus();
private:
	const FFS_ItemData* m_ItemData = NULL;

	UFUNCTION(Category = "UI")
	void OnClicked();
};
