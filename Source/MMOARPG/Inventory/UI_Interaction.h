#pragma once

#include "CoreMinimal.h"
#include "UI_Base.h"

#include "UI_Interaction.generated.h"

class UButton;
class UImage;
struct FFS_ItemData;
class UTextBlock;
//class UUserWidget;

UCLASS()
class UUI_Interaction : public UUI_Base
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	UImage* Img_FillBorder;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* FillAnim;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* Animation;
	
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* LoopingPulse;
	
public:
	virtual void NativeConstruct();

	virtual void NativeDestruct();

	//UFUNCTION(BlueprintCallable, Category = "Interaction UI")
	void SetBorderFill(float Value);

	//UFUNCTION(BlueprintCallable, Category = "Interaction UI")
	void SetFillDecimalValue(float Value);
//
//private:
//	FFS_ItemData* m_ItemData = NULL;
};
