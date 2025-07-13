// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "InventoryHUD.generated.h"

class UUI_UpperUIBar;
/**
 * 
 */
UCLASS()
class AInventoryHUD : public AHUD
{
	GENERATED_BODY()
	
	UPROPERTY()
	TSubclassOf<UUI_UpperUIBar> UpperUIBarClass;
public:
	AInventoryHUD();

	UUI_UpperUIBar* GetWidget();
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
private:
	UUI_UpperUIBar* UpperUIBar;
};
