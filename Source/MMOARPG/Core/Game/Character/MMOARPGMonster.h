// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/MMOARPGCharacterBase.h"
#include "BaseElement.h"
#include "MMOARPGMonster.generated.h"

UENUM()
enum  class ETargetTypes :uint8
{
	Self,
	SelectedEnemy,
	SelectedArea,//AOE
	AreaAroundSelf,//自身周围
	Missile //比如元气弹
};

//伤害类型
UENUM()
enum class EDamageType :uint8 
{
	Magic,
	Physical
};

//效果，比如效果增加，和效果削弱
UENUM()
enum class EEffectiveness :uint8 
{
	Effective,
	SuperEffective,
	LowEffective
};

/**
 * 
 */
UCLASS()
class MMOARPG_API AMMOARPGMonster : public AMMOARPGCharacterBase
{
	GENERATED_BODY()
	
public:
	AMMOARPGMonster();

protected:
	float BaseDamage = 25.0f;
	EDamageType DamageType = EDamageType::Physical;
	UPROPERTY(EditAnywhere, Category = Info)
	TSubclassOf<class ABaseElement> Element;
	int CritChance = 25;

	class AMMOARPGEnemyController* MyController;

	UPROPERTY(EditAnywhere, Category = Info)
	bool bAggressive;//是否是有侵略性的

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = Behavior)
	bool bDead;

	UPROPERTY(VisibleAnywhere, Category = AI)
	class UAIPerceptionComponent* AIPerceptionComp;

	class UAISenseConfig_Sight* SightConfig;

	UPROPERTY(VisibleAnywhere, Category = UI)
	class UWidgetComponent* EnemyWidgetComp;
	UPROPERTY(VisibleAnywhere, Category = UI)
	class USphereComponent* ShowUICollision;

	class UEnemyInfoWidget* EnemyInfoWidget;

	UPROPERTY(EditAnywhere, Category = Info)
	FText Name = FText::FromString("Spider");
	UPROPERTY(EditAnywhere, Category = Info)
	int Level = 1;
	UPROPERTY(EditAnywhere, Category = Info)
	float ExpForKill = 500.0f;
	UPROPERTY(EditAnywhere, Category = Info)
	bool bDoesRespawn = true;
	UPROPERTY(EditAnywhere, Category = Info)
	float RespawnTime = 10;

	UFUNCTION(BlueprintCallable)
	void OnNotifyHit();

	void AttackRay();

	bool bInShowRange;
	//当主角里敌人很近的时候显示信息，很近：与球形碰撞体发生碰撞的时候
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void InitWidgetText();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnSightPerceptionUpdate(const TArray<AActor*>& UpdatedActors);

public:
	FVector StartLocation;//初始的位置
	UPROPERTY(VisibleAnywhere, Category = Hit)
	class UArrowComponent* HitArrow;
	UPROPERTY(EditAnywhere, Category = Info)
	float TotalHealth = 50.0f;

	float CurrentHealth;

	UPROPERTY(EditAnywhere, Category = Behavior)
	TArray<class UAnimMontage*> AttackAnimaions;

	FORCEINLINE bool GetBDead() { return bDead; }

	void UpdateHealthBar();
};

