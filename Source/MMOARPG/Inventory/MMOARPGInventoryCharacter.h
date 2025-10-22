// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NetPlay/BladeIINetCharacter.h"
#include "MMOARPGInventoryCharacter.generated.h"

class UUI_UpperUIBar;
class UUI_EditMap;

UCLASS()
class AMMOARPGInventoryCharacter : public ABladeIINetCharacter
{
	GENERATED_BODY()

	//UPROPERTY(EditDefaultsOnly, Category = UI)
	//TSubclassOf<UUI_EditMap> UIEditMap_BPClass;

public:

	AMMOARPGInventoryCharacter();

	UFUNCTION(BlueprintCallable, Category = "InventorySystem")
	void OpenInventory();

	UFUNCTION(BlueprintCallable, Category = "InventorySystem")
	void OpenQuest();


	UFUNCTION(BlueprintCallable, Category = "InventorySystem")
	void OpenCrafting();


	//UFUNCTION(BlueprintCallable, Category = "MapEditSystem")
	//void OpenMapEdit();

protected:
	virtual void BeginPlay() override;

private:
	UUI_UpperUIBar* m_UpperUI;
	APlayerController* m_PlayerController;
	//UUI_EditMap* m_EditMapUI;
};
