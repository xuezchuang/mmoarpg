// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Instance/Core/MMOARPGAnimInstanceBase.h"
#include "MMOARPGFlyAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class MMOARPG_API UMMOARPGFlyAnimInstance : public UMMOARPGAnimInstanceBase
{
	GENERATED_BODY()

public:
	UMMOARPGFlyAnimInstance();

	virtual void InitAnimInstance(ACharacter* InCharacter);

	virtual void NativeInitializeAnimation();
	virtual void NativeUpdateAnimation(float Deltaseconds);
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimAttrubute")
	FVector FlySpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimAttrubute")
	bool bFastFly;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimAttrubute")
	FVector2D RotationRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimAttrubute")
	EDodgeFly DodgeFly;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimAttrubute")
	bool bLand;
};
