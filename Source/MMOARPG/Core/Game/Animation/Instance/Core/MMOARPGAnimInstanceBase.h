// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MMOARPGGameType.h"
#include "MMOARPGType.h"
#include "MMOARPGAnimInstanceBase.generated.h"

/**
 * 
 */
UCLASS()
class MMOARPG_API UMMOARPGAnimInstanceBase : public UAnimInstance
{
	GENERATED_BODY()

public:
	UMMOARPGAnimInstanceBase();

	virtual void InitAnimInstance(ACharacter *InCharacter);

	virtual void NativeInitializeAnimation();
	virtual void NativeUpdateAnimation(float Deltaseconds);
public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimAttrubute")
	bool bDeath;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimAttrubute")
	float Speed;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimAttrubute")
	bool bInAir;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimAttrubute")
	ECharacterActionState ActionState;

	UPROPERTY(EditDefaultsOnly, Category = "AnimAttrubute|FootIK")
	bool bFootIK;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimAttrubute|FootIK")
	float LeftOffset;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimAttrubute|FootIK")
	float RightOffset;

	UPROPERTY(EditDefaultsOnly, Category = "AnimAttrubute|FootIK")
	FName LeftBoneName;

	UPROPERTY(EditDefaultsOnly, Category = "AnimAttrubute|FootIK")
	FName RightBoneName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimAttrubute|FootIK")
	float ButtZOffset;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category = "AnimAttrubute|FootIK")
	int32 FookIKID;

	//±£¡Ù 
	UPROPERTY(EditDefaultsOnly, Category = "AnimAttrubute|FootIK")
	TArray<FName> BoneNames;

public:
	UFUNCTION(BlueprintPure, BlueprintCallable)
	float GetFootIKOffset(const FName &InBoneName);
};
