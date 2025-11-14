// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/MMOARPGCharacterBase.h"
#include "MMOARPGNPC.generated.h"

class USelectableComponent;
/**
 * 
 */
UCLASS()
class MMOARPG_API AMMOARPGNPC : public AMMOARPGCharacterBase
{
	GENERATED_BODY()
public:
	AMMOARPGNPC();

	virtual void BeginPlay()	override;
protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Selectable")
	USelectableComponent* SelectableComp;


	UFUNCTION(BlueprintNativeEvent, Category = "Selectable")
	void HandleSelected();

	UFUNCTION(BlueprintNativeEvent, Category = "Selectable")
	void HandleSelectionEnd();
};
