// Fill out your copyright notice in the Description page of Project Settings.

#include "MMOARPGEnemyController.h"
#include "Character/MMOARPGMonster.h"
#include "NavigationSystem.h"
#include "TimerManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"


AMMOARPGEnemyController::AMMOARPGEnemyController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(this);
}


void AMMOARPGEnemyController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	EnemyPawn = Cast<AMMOARPGMonster>(InPawn);

	AnimInstance = EnemyPawn->GetMesh()->GetAnimInstance();

	EnemyPawn->GetCharacterMovement()->MaxWalkSpeed = PatrolWalkSpeed;
}


int AMMOARPGEnemyController::GetNextAnimationIndex()
{
	if (CurrentAttackIndex + 1 >= EnemyPawn->AttackAnimaions.Num())
	{
		return 0;
	}
	else
	{
		return (CurrentAttackIndex + 1);
	}
}

bool AMMOARPGEnemyController::BInAttackRange()
{
	if (TargetActor)
	{

		return(EnemyPawn->GetDistanceTo(TargetActor) <= AttackRange);
	}
	else
	{
		return false;
	}
}

void AMMOARPGEnemyController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{

	if (bIsRunningBack)
	{

		bIsRunningBack = false;
		bIsPatrolling = true;
		EnemyPawn->GetCharacterMovement()->MaxWalkSpeed = PatrolWalkSpeed;
	}

	if (BInAttackRange())
	{
		PerformAttack();
		return;
	}

	if (bIsPatrolling)
	{
		float ThinkTime = FMath::RandRange(1, 4);
		GetWorldTimerManager().SetTimer(TimerHandle_Patrol, this, &AMMOARPGEnemyController::DetectedPatrol, ThinkTime, false);
	}
}

void AMMOARPGEnemyController::DetectedPatrol()
{
	if (bIsPatrolling)
	{
		Patrol();
	}
}

void AMMOARPGEnemyController::PerformAttack()
{
	if (!EnemyPawn->GetBDead() && !bIsRunningBack && bWasAggroed)
	{

		bIsPatrolling = false;

		EnemyPawn->GetCharacterMovement()->StopMovementImmediately();

		if (TargetActor)
		{
			FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(EnemyPawn->GetActorLocation(), TargetActor->GetActorLocation());
			EnemyPawn->SetActorRotation(Rotation);
		}

		CurrentAttackMontage = EnemyPawn->AttackAnimaions[CurrentAttackIndex];

		AnimInstance->Montage_Play(CurrentAttackMontage);

		GetWorldTimerManager().SetTimer(TimerHandle_AnimPlayOver, this, &AMMOARPGEnemyController::OnAnimPlayOver, CurrentAttackMontage->GetPlayLength(), false);
	}
}

void AMMOARPGEnemyController::OnAnimPlayOver()
{
	CurrentAttackMontage = nullptr;
	CurrentAttackIndex = GetNextAnimationIndex();

	if (BInAttackRange())
	{
		PerformAttack();
	}
	else
	{
		MoveToActor(TargetActor);
	}
}

void AMMOARPGEnemyController::OnAggroedPulled(AActor* Target)
{
	if (!bWasAggroed && !bIsRunningBack)
	{
		bWasAggroed = true;
		TargetActor = Target;
		bIsPatrolling = false;

		EnemyPawn->GetCharacterMovement()->MaxWalkSpeed = AggroedWalkSpeed;

		GetWorldTimerManager().SetTimer(TimerHnadle_CalDis, this, &AMMOARPGEnemyController::CacualteTargetDistance, 1.0f, true);

		if (BInAttackRange())
		{
			PerformAttack();
		}
		else
		{
			MoveToActor(TargetActor);
		}
	}
}

void AMMOARPGEnemyController::CacualteTargetDistance()
{
	if (TargetActor)
		if (FVector::Dist(EnemyPawn->GetActorLocation(), TargetActor->GetActorLocation()) > MaxDistanceToFollowTarget)
		{
			OnReset();
		}
}

void AMMOARPGEnemyController::OnReset()
{
	AnimInstance->Montage_Stop(0.0f);
	bIsRunningBack = true;
	EnemyPawn->CurrentHealth = EnemyPawn->Info.TotalHealth;
	EnemyPawn->UpdateHealthBar();
	CurrentAttackIndex = 0;
	GetWorldTimerManager().ClearTimer(TimerHandle_Patrol);
	GetWorldTimerManager().ClearTimer(TimerHandle_AnimPlayOver);
	GetWorldTimerManager().ClearTimer(TimerHnadle_CalDis);
	TargetActor = nullptr;
	bWasAggroed = false;
	MoveToLocation(EnemyPawn->StartLocation);
}

void AMMOARPGEnemyController::Patrol()
{
	const float SearchRadius = 1000.0f;
	if (NavSys)
	{
		FNavLocation RandomPt;
		bool bFound = NavSys->GetRandomReachablePointInRadius(EnemyPawn->GetActorLocation(), SearchRadius, RandomPt);
		if (bFound)
		{
			MoveToLocation(RandomPt.Location);
		}
	}
}

