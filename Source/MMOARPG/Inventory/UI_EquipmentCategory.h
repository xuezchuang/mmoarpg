#pragma once

#include "CoreMinimal.h"
#include "../UI/Core/UI_Base.h"
#include "UI_EquipmentCategory.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquipmentCategoryClicked, UUI_EquipmentCategory*, CategoryWidget);

class UButton;
class UTextBlock;

UCLASS(Blueprintable)
class MMOARPG_API UUI_EquipmentCategory : public UUI_Base
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativePreConstruct() override;

	UFUNCTION(BlueprintCallable, Category = "EquipmentCategory")
	void SetCategoryText(const FText& InText);

	UFUNCTION(BlueprintCallable, Category = "EquipmentCategory")
	FText GetCategoryText() const { return CategoryLabel; }

	UFUNCTION(BlueprintCallable, Category = "EquipmentCategory")
	void SetSelected(bool bInSelected);

	UFUNCTION(BlueprintPure, Category = "EquipmentCategory")
	bool IsSelected() const { return bSelected; }

	UPROPERTY(BlueprintAssignable, Category = "EquipmentCategory")
	FOnEquipmentCategoryClicked OnCategoryClicked;

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "EquipmentCategory")
	void BP_OnSelectedChanged(bool bInSelected);

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* Button_82;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* CategoryText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "EquipmentCategory")
	FText CategoryLabel;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "EquipmentCategory")
	bool bSelected = false;

private:
	UFUNCTION()
	void HandleButtonClicked();
};
