// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "MMOARPGEnemyController.generated.h"

/**
 * 
 */
UCLASS()
class MMOARPG_API AMMOARPGEnemyController : public AAIController
{
	GENERATED_BODY()

	class UNavigationSystemV1* NavSys;

	class AMMOARPGMonster* EnemyPawn;

	FTimerHandle TimerHandle_Patrol;


	float AggroedWalkSpeed = 600.0f;//追逐的时候的移动速度

	class UAnimMontage* CurrentAttackMontage;//当前需要播放的攻击动画


	float AttackRange=300.0f;//攻击范围

	float MaxDistanceToFollowTarget=1500;//最大的能够跟随目标的距离


	FTimerHandle TimerHnadle_CalDis;

	AMMOARPGEnemyController();

	int GetNextAnimationIndex();//得到下一个要播放动画的索引，主要是来判断动画数组到头的时候，将索引重新置为0

	bool BInAttackRange();//判断目标是否在敌人攻击范围之内

	virtual void OnPossess(APawn* InPawn) override;

	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;
	
	void DetectedPatrol();

	void PerformAttack();//执行攻击的函数


	FTimerHandle TimerHandle_AnimPlayOver;
	void OnAnimPlayOver();//当攻击动画播放完毕调用


	void  CacualteTargetDistance();

	void OnReset();//当追着目标跑，但是距离目标太远的时候，就不追了，回到初始位置，并将状态置成初始状态
public:
	AActor* TargetActor;//要攻击或追逐的对象
	UAnimInstance* AnimInstance;
	float PatrolWalkSpeed = 200.0f;//巡逻的时候的移动速度
	int CurrentAttackIndex = 0;//当前动画索引
	bool bIsRunningBack;//判断状态：是否是追逐目标但目标脱离了的状态
	bool bIsPatrolling = true;//是否处于追逐状态

	bool bWasAggroed = false;//是否处于狂暴的状态
	void Patrol();


	void OnAggroedPulled(AActor* Target);//当发现目标之后，追逐目标
};
