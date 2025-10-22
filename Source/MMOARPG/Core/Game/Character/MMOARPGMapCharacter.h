// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MMOARPGCharacter.h"
#include "MMOARPGMapCharacter.generated.h"

class UUI_EditMap;

/**
 * 
 */
UCLASS()
class MMOARPG_API AMMOARPGMapCharacter : public AMMOARPGCharacter
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = UI)
	TSubclassOf<UUI_EditMap> UIEditMap_BPClass;
public:

	UFUNCTION(BlueprintCallable, Category = "MapEditSystem")
	void OpenMapEdit();

protected:
	virtual void BeginPlay() override;

private:
	UUI_EditMap* m_EditMapUI;
};
