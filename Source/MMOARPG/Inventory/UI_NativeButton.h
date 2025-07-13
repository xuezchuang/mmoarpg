#pragma once

#include "CoreMinimal.h"
#include "UI_Base.h"

#include "UI_NativeButton.generated.h"

class UButton;
//class UUserWidget;

UCLASS()
class UUI_NativeButton: public UUI_Base
{
	GENERATED_BODY()
//protected:
public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* Native_Button;
public:
	virtual void NativeConstruct();

	virtual void NativeDestruct();

};
