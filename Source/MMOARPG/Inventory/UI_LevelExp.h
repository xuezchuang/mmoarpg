#pragma once

#include "CoreMinimal.h"
#include "UI_Base.h"
#include "UI_LevelExp.generated.h"

class UProgressBar;
class UTextBlock;

UCLASS()
class UUI_LevelExp : public UUI_Base
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "LevelExp")
	UTextBlock* Level;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "LevelExp")
	UTextBlock* CurrentExp;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "LevelExp")
	UTextBlock* RequiredExp;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "LevelExp")
	UProgressBar* ProgressBar;

	UFUNCTION(BlueprintCallable, Category = "LevelExp")
	void RefreshDisplay();

private:
	void ResolveFallbackBindings();
	void RecvProtocol(uint32 ProtocolNumber);
	int32 ResolveRequiredExp(int32 InLevel, int32 InCurrentExp) const;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LevelExp", meta = (AllowPrivateAccess = "true", ClampMin = "0"))
	int32 DefaultRequiredExp = 600;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UTextBlock>> ExtraLevelTextBlocks;

	TArray<uint32> InterestingProtos;
	TArray<FDelegateHandle> InterestingHandles;
};
