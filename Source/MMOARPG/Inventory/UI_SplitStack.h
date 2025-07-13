#pragma once

#include "CoreMinimal.h"
#include "UI_Base.h"
#include "UI_SplitStack.generated.h"

class UButton;
class UVerticalBox;
struct FFS_ItemData;
//class UUserWidget;

UCLASS()
class UUI_SplitStack: public UUI_Base
{
	GENERATED_BODY()
public:
//	UPROPERTY(meta = (BindWidget))
//	UButton* Button_Buy;
//
//	UPROPERTY(meta = (BindWidget))
//	UVerticalBox* VerticalBox_Shop;

public:
	virtual void NativeConstruct();

	virtual void NativeDestruct();

	//UFUNCTION()
	//void OnClicked();
	//void UnClicked();
private:

	//void UpdateItem(FFS_ItemData* ItemData);

	//FFS_ItemData* m_SelItemData;
};
