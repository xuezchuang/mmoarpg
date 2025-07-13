// Fill out your copyright notice in the Description page of Project Settings.

#include "FlyComponent.h"
#include "../Game/Character/Core/MMOARPGCharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "MMOARPGType.h"
// Sets default values for this component's properties
UFlyComponent::UFlyComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	// ...
}


// Called when the game starts
void UFlyComponent::BeginPlay()
{
	Super::BeginPlay();

	MMOARPGCharacterBase = Cast<AMMOARPGCharacterBase>(GetOwner());
	if (MMOARPGCharacterBase.IsValid())
	{
		CharacterMovementComponent = Cast<UCharacterMovementComponent>(MMOARPGCharacterBase->GetMovementComponent());	
		CapsuleComponent = MMOARPGCharacterBase->GetCapsuleComponent();
		CameraComponent = MMOARPGCharacterBase->GetFollowCamera();

		if (CharacterMovementComponent.IsValid())
		{
			CharacterMovementComponent->MaxAcceleration = 2500.f;
			CharacterMovementComponent->BrakingDecelerationFlying = 1400.f;
		}

		CapsuleComponent->OnComponentHit.AddDynamic(this, &UFlyComponent::Landed);

		bFastFly.Fun.BindLambda([&](){DodgeFly = EDodgeFly::DODGE_NONE;});
		bLand.Fun.BindLambda([&]() 
		{
			if (MMOARPGCharacterBase.IsValid())
			{
				MMOARPGCharacterBase->ResetActionState(ECharacterActionState::FLIGHT_STATE);
				ResetFly();
		}});
	}

	// ...
}

void UFlyComponent::Landed(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (MMOARPGCharacterBase.IsValid())
	{
		if (MMOARPGCharacterBase->GetActionState() == ECharacterActionState::FLIGHT_STATE &&
			bFastFly)
		{
			float CosValue = FVector::DotProduct(CapsuleComponent->GetForwardVector(), Hit.ImpactNormal);
			float CosAngle = (180.f) / PI * FMath::Acos(CosValue);
			if (CosAngle >= 125.f)
			{			
				if (Hit.ImpactNormal.Z > 0.5f)
				{
					Reset();

					bLand = true;
					bLand = 1.6f;
				}
			}
		}
	}
}

void UFlyComponent::Print(float InTime,const FString& InString)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, InTime, FColor::Red, FString::Printf(TEXT("%s"), *InString));
	}
}

void UFlyComponent::Reset()
{
	CharacterMovementComponent->bOrientRotationToMovement = true;
	CharacterMovementComponent->MaxFlySpeed = 600.f;
	CharacterMovementComponent->SetMovementMode(EMovementMode::MOVE_Walking);

	FRotator NewRot = MMOARPGCharacterBase->GetActorRotation();
	NewRot.Pitch = 0.f;
	MMOARPGCharacterBase->SetActorRotation(NewRot);
}

void UFlyComponent::ResetFly()
{
	if (CharacterMovementComponent.IsValid() && MMOARPGCharacterBase.IsValid())
	{ 
		if (MMOARPGCharacterBase->GetActionState() == ECharacterActionState::FLIGHT_STATE)
		{
			CharacterMovementComponent->bOrientRotationToMovement = false;
			CharacterMovementComponent->SetMovementMode(EMovementMode::MOVE_Flying);
		}
		else
		{
			Reset();
		}

		bFastFly = false;
	}
}

void UFlyComponent::ResetFastFly()
{
	if (CharacterMovementComponent.IsValid())
	{
		if (bFastFly)
		{
			bFastFly = false;
			CharacterMovementComponent->MaxFlySpeed = 600.f;
		}
		else
		{
			bFastFly = true;
			CharacterMovementComponent->MaxFlySpeed = 2000.f;
		}
	}
}

void UFlyComponent::ResetDodgeFly(EDodgeFly InFlyState)
{
	if (bFastFly)
	{
		DodgeFly = InFlyState;

		//时间重置
		bFastFly = 1.6f;
	}
}

// Called every frame
void UFlyComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (CharacterMovementComponent.IsValid() &&
		MMOARPGCharacterBase.IsValid() &&
		CapsuleComponent.IsValid() &&
		CameraComponent.IsValid())
	{
		if (MMOARPGCharacterBase->GetActionState() == ECharacterActionState::FLIGHT_STATE)
		{
			if (MMOARPGCharacterBase->GetLocalRole() == ENetRole::ROLE_Authority ||
				MMOARPGCharacterBase->GetLocalRole() == ENetRole::ROLE_AutonomousProxy)
			{
				if (!bLand)
				{
					//设置角色旋转
					FRotator CameraRotator = CameraComponent->GetComponentRotation();
					FRotator CapsuleRotator = CapsuleComponent->GetComponentRotation();

					//修正pitch
					if (!bFastFly)
					{
						CameraRotator.Pitch = 0.f;
					}

					FRotator NewRot = FMath::RInterpTo(CapsuleRotator, CameraRotator, DeltaTime, 8.f);

					MMOARPGCharacterBase->SetActorRotation(NewRot);

					//设置角速度作映射-1 1
					if (1)
					{
						float PreFrameNum = 1.f / DeltaTime;

						FRotator NewDeltaTimeRot = MMOARPGCharacterBase->GetActorRotation() - LastRotator;
						NewDeltaTimeRot *= PreFrameNum;

						//Print(DeltaTime, NewDeltaTimeRot.ToString());
						RotationRate.X = FMath::GetMappedRangeValueClamped(FVector2D(-360.f, 360.f), FVector2D(-1.f, 1.f), NewDeltaTimeRot.Yaw);
						RotationRate.Y = FMath::GetMappedRangeValueClamped(FVector2D(-360.f, 360.f), FVector2D(-1.f, 1.f), NewDeltaTimeRot.Pitch);
						LastRotator = MMOARPGCharacterBase->GetActorRotation();
					}
					else
					{
						//设置角速度作映射-1 1
						FVector PhysicsAngularVelocityInDegrees = CapsuleComponent->GetPhysicsAngularVelocityInDegrees();

						//Print(DeltaTime, PhysicsAngularVelocityInDegrees.ToString());

						RotationRate.X = FMath::GetMappedRangeValueClamped(FVector2D(-360.f, 360.f), FVector2D(-1.f, 1.f), PhysicsAngularVelocityInDegrees.Y);

					}
				}
			}
		}

		//记录DodgeFly时间
		bFastFly.Tick(DeltaTime);

		//登录的时间
		bLand.Tick(DeltaTime);
	}
}

void UFlyComponent::FlyForwardAxis(float InAxisValue)
{
	if (CharacterMovementComponent.IsValid() &&
		MMOARPGCharacterBase.IsValid() &&
		CapsuleComponent.IsValid() &&
		CameraComponent.IsValid())
	{
		if (bFastFly)
		{
			const FVector Direction = CameraComponent->GetForwardVector();
			MMOARPGCharacterBase->AddMovementInput(Direction, 1.f);
		}
		else
		{
			const FVector Direction = CameraComponent->GetForwardVector();
			MMOARPGCharacterBase->AddMovementInput(Direction, InAxisValue);
		}
	}
}