// Fill out your copyright notice in the Description page of Project Settings.

#include "MMOARPGFlyAnimInstance.h"
#include "../../Character/Core/MMOARPGCharacterBase.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../../../Component/FlyComponent.h"

UMMOARPGFlyAnimInstance::UMMOARPGFlyAnimInstance()
	:Super()
	,DodgeFly(EDodgeFly::DODGE_NONE)
{

}

void UMMOARPGFlyAnimInstance::InitAnimInstance(ACharacter* InCharacter)
{
	Super::InitAnimInstance(InCharacter);

}

void UMMOARPGFlyAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
}

void UMMOARPGFlyAnimInstance::NativeUpdateAnimation(float Deltaseconds)
{
	Super::NativeUpdateAnimation(Deltaseconds);

	if (AMMOARPGCharacterBase* InCharacterBase = Cast<AMMOARPGCharacterBase>(TryGetPawnOwner()))
	{
		FVector SpeedVector = InCharacterBase->GetVelocity();
		FRotator CharacterRot =  InCharacterBase->GetActorRotation();
		
		SpeedVector = CharacterRot.UnrotateVector(SpeedVector);

		if (UCharacterMovementComponent* InCharacterMovementComponent = Cast<UCharacterMovementComponent>(InCharacterBase->GetMovementComponent()))
		{
			float MaxFlySpeed = InCharacterMovementComponent->MaxFlySpeed;

			FlySpeed.X = FMath::GetMappedRangeValueClamped(FVector2D(-MaxFlySpeed, MaxFlySpeed), FVector2D(-1.f, 1.f), SpeedVector.X);
			FlySpeed.Y = FMath::GetMappedRangeValueClamped(FVector2D(-MaxFlySpeed, MaxFlySpeed), FVector2D(-1.f, 1.f), SpeedVector.Y);
			FlySpeed.Z = FMath::GetMappedRangeValueClamped(FVector2D(-MaxFlySpeed, MaxFlySpeed), FVector2D(-1.f, 1.f), SpeedVector.Z);
		}

		//¹ØÁª
		RotationRate = InCharacterBase->GetFlyComponent()->RotationRate;
		bFastFly = *InCharacterBase->GetFlyComponent()->bFastFly;
		bLand = *InCharacterBase->GetFlyComponent()->bLand;
		DodgeFly = InCharacterBase->GetFlyComponent()->DodgeFly;
	}
}
