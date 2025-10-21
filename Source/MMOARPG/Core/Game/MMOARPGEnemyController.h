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


	float AggroedWalkSpeed = 600.0f;//׷���ʱ����ƶ��ٶ�

	class UAnimMontage* CurrentAttackMontage;//��ǰ��Ҫ���ŵĹ�������


	float AttackRange=300.0f;//������Χ

	float MaxDistanceToFollowTarget=1500;//�����ܹ�����Ŀ��ľ���


	FTimerHandle TimerHnadle_CalDis;

	AMMOARPGEnemyController();

	int GetNextAnimationIndex();//�õ���һ��Ҫ���Ŷ�������������Ҫ�����ж϶������鵽ͷ��ʱ�򣬽�����������Ϊ0

	bool BInAttackRange();//�ж�Ŀ���Ƿ��ڵ��˹�����Χ֮��

	virtual void OnPossess(APawn* InPawn) override;

	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;
	
	void DetectedPatrol();

	void PerformAttack();//ִ�й����ĺ���


	FTimerHandle TimerHandle_AnimPlayOver;
	void OnAnimPlayOver();//����������������ϵ���


	void  CacualteTargetDistance();

	void OnReset();//��׷��Ŀ���ܣ����Ǿ���Ŀ��̫Զ��ʱ�򣬾Ͳ�׷�ˣ��ص���ʼλ�ã�����״̬�óɳ�ʼ״̬
public:
	AActor* TargetActor;//Ҫ������׷��Ķ���
	UAnimInstance* AnimInstance;
	float PatrolWalkSpeed = 200.0f;//Ѳ�ߵ�ʱ����ƶ��ٶ�
	int CurrentAttackIndex = 0;//��ǰ��������
	bool bIsRunningBack;//�ж�״̬���Ƿ���׷��Ŀ�굫Ŀ�������˵�״̬
	bool bIsPatrolling = true;//�Ƿ���׷��״̬

	bool bWasAggroed = false;//�Ƿ��ڿ񱩵�״̬
	void Patrol();


	void OnAggroedPulled(AActor* Target);//������Ŀ��֮��׷��Ŀ��
};
