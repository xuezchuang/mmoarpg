#pragma once

#include "CoreMinimal.h"
#include "UI_Base.h"
#include "UI_GoldWeightBar.generated.h"

class UTextBlock;

UCLASS()
class UUI_GoldWeightBar : public UUI_Base
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "GoldWeightBar")
	UTextBlock* Gold;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "GoldWeightBar")
	UTextBlock* CurrentWeight;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "GoldWeightBar")
	UTextBlock* MaxWeight;

	UFUNCTION(BlueprintCallable, Category = "GoldWeightBar")
	void RefreshDisplay();

private:
	void ResolveFallbackBindings();
	void RecvProtocol(uint32 ProtocolNumber);
	float ResolveCurrentWeight();
	float ResolveMaxWeight() const;
	static FText FormatWeightText(float InValue);

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GoldWeightBar", meta = (AllowPrivateAccess = "true", ClampMin = "0"))
	int32 MaxDisplayGold = MAX_int32;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GoldWeightBar", meta = (AllowPrivateAccess = "true", ClampMin = "0.0"))
	float DefaultMaxWeight = 999.f;

	TArray<uint32> InterestingProtos;
	TArray<FDelegateHandle> InterestingHandles;
};
