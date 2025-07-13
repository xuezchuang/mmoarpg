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
	//��ȡ��һ�����߼����
	bool GetTraceImpactPointAndNormal(FVector &OutInitalTraceImpactPoint, FVector& OutInitalTraceNormal);

protected:

	//����ɫ������λ���ƶ�������
	FVector GetCalpsuleBaseLocation(float InZOffset);

	//�����ɫ�ƶ��ķ���
	FVector GetPlayerMovementInput();
	
	//��ȡǰ��������
	bool GetControlForwardRightVector(FVector &InForwardVector,FVector &InRightVector);
protected:
	TWeakObjectPtr<ACharacter> Character;
	TWeakObjectPtr<UCapsuleComponent> CapsuleComponent;
	TWeakObjectPtr<UCharacterMovementComponent> CharacterMovementComponent;

	FTraceSettings TraceSettings;
	FName ForwardKeyName;
	FName RightKeyName;
};