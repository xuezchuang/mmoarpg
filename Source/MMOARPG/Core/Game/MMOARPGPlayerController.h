// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MMOARPGPlayerController.generated.h"

class AMMOARPGCharacter;
/**
 * 
 */
UCLASS()
class MMOARPG_API AMMOARPGPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AMMOARPGPlayerController();

	UFUNCTION(server, reliable)
	void ReplaceCharacter(int32 InCharacterID);

protected:
	virtual void BeginPlay() override;
};
