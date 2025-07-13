#pragma once

#include "CoreMinimal.h"
#include "UI_Base.h"

#include "UI_Button01_Content.generated.h"

class UTextBlock;
class UFontFace;
class UFont;
class UHorizontalBox;
//class UUserWidget;

UCLASS()
class UUI_Button01_Content: public UUI_Base
{
	GENERATED_BODY()
//protected:
public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* ButtonText;

	//UPROPERTY(meta = (BindWidget))
	//UHorizontalBox* HB_Text;
	//
public:
	virtual void NativeConstruct();

	virtual void NativeDestruct();

	void UpdateFontInfo(UFontFace* FontFace, UFont* Font, int32 FontSize, FLinearColor LinearColor);
	void UpdateText(FText Text);
	void UpdateJustification(EHorizontalAlignment Alignment);
};
