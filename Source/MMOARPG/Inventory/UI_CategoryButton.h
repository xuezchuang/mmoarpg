#pragma once

#include "CoreMinimal.h"
#include "UI_Base.h"
#include "Engine\Texture2D.h"
#include "UObject\NoExportTypes.h"
#include "UI_CategoryButton.generated.h"

class UButton;
class UImage;

UCLASS()
class UUI_CategoryButton : public UUI_Base
{
	GENERATED_BODY()

	//UPROPERTY(EditInstanceOnly, Category = "Default")
	//TObjectPtr<UTexture2D> CategoryIcon;

	//UPROPERTY(EditInstanceOnly, Category = "Default")
	//FLinearColor IconColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);

	//UPROPERTY(meta = (BindWidget))
	//UImage* Preview_Icon;

	//protected:
public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* Button_Weapons;

	UFUNCTION()
	void TestButton();
public:
	virtual void NativeConstruct();

	virtual void NativeDestruct();

protected:
	virtual void NativePreConstruct() override;

};
