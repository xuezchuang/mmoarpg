// Copyright (C) RenZhai.2020.All Rights Reserved.

#include "Core/Element/MantleCheck/SimpleMantleCheckSystem.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
//#include "Camera/CameraComponent.h"
//#include "Components/SkeletalMeshComponent.h"
#include "Core/Element/MantleCheck/SimpleMantleCheck.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"

void FSimpleMantleCheckSystem::Init(
	ACharacter* InCharacter,
	const FName& InForwardKeyName,
	const FName& InRightKeyName)
{
	ForwardKeyName = InForwardKeyName;
	RightKeyName = InRightKeyName;

	Character = InCharacter;

	if (Character.IsValid())
	{
		CharacterMovementComponent = Cast<UCharacterMovementComponent>(Character->GetMovementComponent());
		CapsuleComponent = Character->GetCapsuleComponent();
	}
}

void FSimpleMantleCheckSystem::Tick(float DeltaTime)
{

}

FVector FSimpleMantleCheckSystem::GetCalpsuleBaseLocation(float InZOffset)
{
	if (CapsuleComponent.IsValid())
	{
		float Length = CapsuleComponent->GetScaledCapsuleHalfHeight() + InZOffset;
		FVector UPLength = Length * CapsuleComponent->GetUpVector();

		//让坐标从角色的脚下位置开始算起而不是中间
		return CapsuleComponent->GetComponentLocation() - UPLength;
	}

	return FVector::ZeroVector;
}

bool FSimpleMantleCheckSystem::GetControlForwardRightVector(FVector& OutForwardVector, FVector& OutRightVector)
{
	if (Character.IsValid())
	{
		FRotator ControlRotation =  Character->GetControlRotation();
		OutForwardVector = UKismetMathLibrary::GetForwardVector(FRotator(0.f, 0.f, ControlRotation.Yaw));
		OutRightVector = UKismetMathLibrary::GetRightVector(FRotator(0.f, 0.f, ControlRotation.Yaw));

		return true;
	}
	
	return false;
}

FVector FSimpleMantleCheckSystem::GetPlayerMovementInput()
{
	FVector MovementInput = FVector::ZeroVector;
	if (Character.IsValid())
	{
		FVector ForwardDir = FVector::ZeroVector;
		FVector RightDir= FVector::ZeroVector;

		float ForwardValue = Character->GetInputAxisValue(ForwardKeyName);
		float RightValue = Character->GetInputAxisValue(RightKeyName);

		if (GetControlForwardRightVector(ForwardDir, RightDir))
		{
			MovementInput = ForwardValue* ForwardDir + RightValue * RightDir;
			MovementInput.Normalize();
		}
	}

	return MovementInput;
}

void FSimpleMantleCheckSystem::MantleCheck()
{
	if (Character.IsValid())
	{
		FVector InitalTraceImpactPoint = FVector::ZeroVector;
		FVector InitalTraceNormal = FVector::ZeroVector;
		if (GetTraceImpactPointAndNormal(InitalTraceImpactPoint, InitalTraceNormal))
		{

		}
	}
}

bool FSimpleMantleCheckSystem::GetTraceImpactPointAndNormal(FVector& OutInitalTraceImpactPoint, FVector& OutInitalTraceNormal)
{
	FVector MovementInput = GetPlayerMovementInput();

	//求出障碍物的平均高度
	float AveragePoint = (TraceSettings.MaxLedgeHeight + TraceSettings.MinLedgeHeight) * 0.5f;
	float HelfDistance = (TraceSettings.MaxLedgeHeight - TraceSettings.MinLedgeHeight) * 0.5f;

	//按照自己按键输出的位置向后退30
	FVector TraceStart = MovementInput * (-30.f) + GetCalpsuleBaseLocation(2.f) +
		FVector(0.f, 0.f, AveragePoint);

	//在原来企图要走的位置增加一个搜索范围
	FVector TraceEnd = MovementInput * TraceSettings.ReachDistance + TraceStart;

	FHitResult OutHit;
	TArray<AActor*> ActorsToIgnore;
	if (UKismetSystemLibrary::CapsuleTraceSingle(
		Character->GetWorld(),
		TraceStart,
		TraceEnd,
		TraceSettings.ForwardTraceRadius,
		HelfDistance,
		ETraceTypeQuery::TraceTypeQuery1,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::ForDuration,
		OutHit,
		true))
	{
		if (CharacterMovementComponent.IsValid())
		{
			if (!CharacterMovementComponent->IsWalkable(OutHit))
			{
				OutInitalTraceImpactPoint = OutHit.ImpactPoint;
				OutInitalTraceImpactPoint = OutHit.ImpactNormal;

				return true;
			}
		}
	}

	return false;
}
