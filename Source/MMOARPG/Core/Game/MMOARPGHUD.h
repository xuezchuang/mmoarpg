// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MMOARPGHUD.generated.h"

class UUI_GameMain;
/**
 * 
 */
UCLASS()
class MMOARPG_API AMMOARPGHUD : public AHUD
{
	GENERATED_BODY()

	UPROPERTY()
	TSubclassOf<UUI_GameMain> GameMainClass;
public:
	AMMOARPGHUD();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
private:
	UUI_GameMain* GameMain;
};
