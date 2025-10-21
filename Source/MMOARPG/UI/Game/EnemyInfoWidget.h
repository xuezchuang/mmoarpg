// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EnemyInfoWidget.generated.h"

/**
 * 
 */
UCLASS()
class MMOARPG_API UEnemyInfoWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(Meta=(BindWidget))
	class UTextBlock* LevelName;
	UPROPERTY(Meta = (BindWidget))
	class UProgressBar* HealthBar;
};
