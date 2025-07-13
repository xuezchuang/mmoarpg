// Copyright (C) RenZhai.2020.All Rights Reserved.
#pragma once
#include "CoreMinimal.h"

class UCapsuleComponent;
class ACharacter;
class UCharacterMovementComponent;

class FSimpleMantleCheckSystem
{
public:
	void Init(ACharacter *InCharacter,const FName &InForwardKeyName, const FName& InRightKeyName);
	
	void Tick(float DeltaTime);

	void MantleCheck();

protected:
	//获取第一个射线检测结果
	bool GetTraceImpactPointAndNormal(FVector &OutInitalTraceImpactPoint, FVector& OutInitalTraceNormal);

protected:

	//将角色的中心位置移动到脚下
	FVector GetCalpsuleBaseLocation(float InZOffset);

	//求出角色移动的方向
	FVector GetPlayerMovementInput();
	
	//获取前进和左右
	bool GetControlForwardRightVector(FVector &InForwardVector,FVector &InRightVector);
protected:
	TWeakObjectPtr<ACharacter> Character;
	TWeakObjectPtr<UCapsuleComponent> CapsuleComponent;
	TWeakObjectPtr<UCharacterMovementComponent> CharacterMovementComponent;

	FTraceSettings TraceSettings;
	FName ForwardKeyName;
	FName RightKeyName;
};