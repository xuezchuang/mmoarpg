#pragma once

#include "CoreMinimal.h"
#include "UI_Base.h"

#include "UI_CraftingListSpacer.generated.h"

class UButton;
//class UBorder;
class UImage;
class UTextBlock;
//class UUserWidget;

UCLASS()
class UUI_CraftingListSpacer: public UUI_Base
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* BG_Image;
private:
	UPROPERTY(meta = (BindWidget))
	UButton* Native_Button;

	UPROPERTY(meta = (BindWidget))
	UImage* Triangle;



	UPROPERTY(VisibleDefaultsOnly, Category = "UISetting")
	FLinearColor VisibleDefaultsOnlyTest;

	UPROPERTY(EditDefaultsOnly, Category = "UISetting")
	FLinearColor BG_Hovered_Color;

	UPROPERTY(EditDefaultsOnly, Category = "UISetting")
	FLinearColor BG_UnHovered_Color;

	UPROPERTY(EditInstanceOnly, Category = "UISetting")
	FText Title;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextBlock_Title;

	UPROPERTY(EditInstanceOnly, Category = "UISetting")
	UWidget* AssociatedBox;
public:
	virtual void NativeConstruct();

	virtual void NativeDestruct();

	
	UWidget* GetBox();
private:
	UFUNCTION()
	void OnClicked();

	UFUNCTION()
	void OnHovered();

	UFUNCTION()
	void OnUnHovered();
private:
	bool bFlip = true;
protected:
	virtual void NativePreConstruct() override;

};
