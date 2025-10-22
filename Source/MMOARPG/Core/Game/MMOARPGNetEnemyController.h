// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "MMOARPGEnemyController.generated.h"

// AMMOARPGNetEnemyController.h

UCLASS()
class MMOARPG_API AMMOARPGNetEnemyController : public AAIController
{
    GENERATED_BODY()

public:
    AMMOARPGNetEnemyController();

    virtual void OnPossess(APawn* InPawn) override;
    virtual void OnUnPossess() override;

    // ��������
private:
    UPROPERTY()
    UNavigationSystemV1* NavSys = nullptr;

    UPROPERTY()
    class AMMOARPGMonster* EnemyPawn = nullptr;

    UPROPERTY()
    UAnimInstance* AnimInstance = nullptr;

    int32 NetMonsterId = 0;
    FVector HomeLocation = FVector::ZeroVector;

public:
    FORCEINLINE void SetNetMonsterId(int32 InId) { NetMonsterId = InId; }
    FORCEINLINE int32 GetNetMonsterId() const { return NetMonsterId; }

    // ����������ָ�ֻ������/�ƶ�/��ֵ���£�
public:
    void Net_SetTarget(AActor* NewTarget);                            // Ŀ���л�
    void Net_MoveTo(const FVector& Dest, float Speed, bool bChasing); // �ƶ�����
    void Net_FaceTo(const FVector& WorldPos);                         // �����
    void Net_PlayAttack(int32 AttackIndex, float PlayRate, float ExpectedSecs);
    void Net_StopAttack();
    void Net_ResetToHome(const FVector& InHome);
    void Net_HealthUpdate(int32 CurHP, int32 MaxHP);
    void Net_Die();
    void Net_Despawn();

private:
    // ���ؽ�ά����С״̬���ڻط�
    UPROPERTY()
    AActor* TargetActor = nullptr;

    int32 CurrentAttackIndex = 0;
    UPROPERTY()
    UAnimMontage* CurrentAttackMontage = nullptr;

    FTimerHandle TimerHandle_AttackEnd; // �����ڡ��Ӿ����롱�����߼�
};

