//Copyright (C) RenZhai.2022.All Rights Reserved.
#include "Hit/Core/ComboSkillHitCollision.h"
#include "Components/SceneComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "CombatInterface/SimpleCombatInterface.h"
#include "Components/SplineComponent.h"

AHitCollision::AHitCollision(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	HitCollisionRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("CollisionRootComponent"));
	RootComponent = HitCollisionRootComponent;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));

	ProjectileMovement->MaxSpeed = 2000.f;
	ProjectileMovement->InitialSpeed = 1600.f;
	ProjectileMovement->ProjectileGravityScale = 0.f;
	ProjectileMovement->UpdatedComponent = RootComponent;

	InitialLifeSpan = 4.f;
	bNetLoadOnClient = false;
	bReplicates = false;

	//近程攻击
	HitCollisionType = EHitCollisionType::HITCOLLISIONTYPE_SHORT_RANGE_ATTACK;

	CurrentSplineTime = 0.f;
	bRandomDirection = false;
}

void AHitCollision::PreInitCollision(AActor* InMyInstigator)
{
	if (!InMyInstigator)
	{
		return;
	}

	switch (HitCollisionType)
	{
		case EHitCollisionType::HITCOLLISIONTYPE_SHORT_RANGE_ATTACK:
		case EHitCollisionType::HITCOLLISIONTYPE_TRACK_LINE:
		{	
			ProjectileMovement->MaxSpeed = 0.f;
			ProjectileMovement->InitialSpeed = 0.f;
			ProjectileMovement->ProjectileGravityScale = 0.f;
			break;
		}
	}

	switch (HitCollisionType)
	{
	case EHitCollisionType::HITCOLLISIONTYPE_SHORT_RANGE_ATTACK:
	{
		break;
	}
	case EHitCollisionType::HITCOLLISIONTYPE_DIRECT_LINE:
		break;
	case EHitCollisionType::HITCOLLISIONTYPE_LINE:
		break;
	case EHitCollisionType::HITCOLLISIONTYPE_TRACK_LINE:
	{
		FVector TargetLocation = FVector::ZeroVector;
		if (ISimpleCombatInterface* InInstigator = Cast<ISimpleCombatInterface>(InMyInstigator))
		{
			if (AActor* InTaget = InInstigator->GetTarget())
			{
				TargetLocation = InTaget->GetActorLocation();
			}
		}
		else //虚拟出来一个
		{
			if (InMyInstigator)
			{
				TargetLocation = InMyInstigator->GetActorForwardVector() * 1000.f + InMyInstigator->GetActorLocation();
			}
		}

		Spline = NewObject<USplineComponent>(this, TEXT("SplineInstance"));
		Spline->RegisterComponent();

		Spline->SetLocationAtSplinePoint(0, GetActorLocation(), ESplineCoordinateSpace::Local);

		DistanceVector = TargetLocation - GetActorLocation();
		auto Position = (InMyInstigator->GetActorForwardVector() * (DistanceVector.Size() / 2.f)) + InMyInstigator->GetActorLocation();

		Position.Z = (DistanceVector.Size() *.5f) * 0.5f;
		Position.Y = bRandomDirection ? FMath::RandRange(0.f, (float)Position.Y) : SplineOffset;

		Spline->SetLocationAtSplinePoint(1, Position, ESplineCoordinateSpace::Local);//中点
		Spline->AddSplinePoint(TargetLocation, ESplineCoordinateSpace::Local);//终点

		break;
	}
	case EHitCollisionType::HITCOLLISIONTYPE_RANGE_LINE:
		break;
	case EHitCollisionType::HITCOLLISIONTYPE_RANGE:
		break;
	case EHitCollisionType::HITCOLLISIONTYPE_CHAIN:
		break;
	}
}

void AHitCollision::HandleDamage(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ACharacter *InCharacter = Cast<ACharacter>(OtherActor))
	{
		AttackedTarget.AddUnique(InCharacter);
	}
}

UPrimitiveComponent* AHitCollision::GetHitDamage()
{
	return NULL;
}

void AHitCollision::SetHitDamageRelativePosition(const FVector& InNewPostion)
{
	if (UPrimitiveComponent* InHitComponent = GetHitDamage())
	{
		InHitComponent->SetRelativeLocation(InNewPostion);
	}
}

void AHitCollision::Collision(bool bCollision)
{
	if (UPrimitiveComponent* InHitComponent = GetHitDamage())
	{
		if (bCollision)
		{
			InHitComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		}
		else
		{
			InHitComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
}

bool AHitCollision::IsExist(ACharacter* InNewTaget) const
{
	//return AttackedTarget.Find(InNewTaget) != INDEX_NONE;

	for (auto &Tmp : AttackedTarget)
	{
		if (Tmp.IsValid())
		{
			if (Tmp == InNewTaget)
			{
				return true;
			}
		}
	}

	return false;
}

// Called when the game starts or when spawned
void AHitCollision::BeginPlay()
{
	Super::BeginPlay();

	if (UPrimitiveComponent *InHitComponent = GetHitDamage())
	{
		//Collision(false);

		InHitComponent->SetHiddenInGame(true);
		InHitComponent->OnComponentBeginOverlap.AddDynamic(this,&AHitCollision::HandleDamage);
	}
}

// Called every frame
void AHitCollision::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	switch (HitCollisionType)
	{
	case EHitCollisionType::HITCOLLISIONTYPE_SHORT_RANGE_ATTACK:
			break;
		case EHitCollisionType::HITCOLLISIONTYPE_DIRECT_LINE:
			break;
		case EHitCollisionType::HITCOLLISIONTYPE_LINE:
			break;
		case EHitCollisionType::HITCOLLISIONTYPE_TRACK_LINE:
		{			
			if (Spline)
			{
				CurrentSplineTime += DeltaTime;
					
				float Distance = Spline->GetSplineLength() * CurrentSplineTime /(DistanceVector.Size() / 1000.f);
				FVector Loction = Spline->GetWorldLocationAtDistanceAlongSpline(Distance);
				FRotator Rotator = Spline->GetRotationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::Local);
					
				SetActorLocationAndRotation(Loction, Rotator);

				if (ISimpleCombatInterface* InInstigator = Cast<ISimpleCombatInterface>(GetInstigator()))
				{
					if (AActor *InTarget = InInstigator->GetTarget())
					{
						float MyDistance = FVector::Distance(InTarget->GetActorLocation(),GetActorLocation());
						if (MyDistance < 50.f)
						{
							Destroyed();
						}
					}
				}
			}			
			break;
		}
		case EHitCollisionType::HITCOLLISIONTYPE_RANGE_LINE:
			break;
		case EHitCollisionType::HITCOLLISIONTYPE_RANGE:
			break;
		case EHitCollisionType::HITCOLLISIONTYPE_CHAIN:
			break;
	}
}