#pragma once

#include "CoreMinimal.h"
#include "UI_Base.h"

#include "UI_Button01_FX.generated.h"

class UButton;
class UBorder;
//class UUserWidget;

UCLASS()
class UUI_Button01_FX: public UUI_Base
{
	GENERATED_BODY()
public:
	UPROPERTY(meta = (BindWidget))
	UBorder* b_Content_Clicked;
public:
	virtual void NativeConstruct();

	virtual void NativeDestruct();

	void OnClicked();
	void UnClicked();
private:
	bool bIsClicked = false;
};
