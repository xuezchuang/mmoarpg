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

USTRUCT(BlueprintType)
struct FMonsterInfo     // 运行时缓存，用于 UI/逻辑
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) FText Name;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) int32 Level = 1;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) float ExpForKill = 0.f;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) bool bDoesRespawn = true;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) float RespawnTime = 0.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) float TotalHealth = 0.f;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) float BaseDamage = 0.f;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) int32 CritChance = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TSubclassOf<class ABaseElement> Element;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) EDamageType DamageType = EDamageType::Physical;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) bool bAggressive = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) bool bDead = false;
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

	//float BaseDamage = 25.0f;
	//EDamageType DamageType = EDamageType::Physical;
	//UPROPERTY(EditAnywhere, Category = Info)
	//TSubclassOf<class ABaseElement> Element;
	//int CritChance = 25;

	class AMMOARPGEnemyController* MyController;

	//UPROPERTY(EditAnywhere, Category = Info)
	//bool bAggressive;//是否是有侵略性的

	//UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = Behavior)
	//bool bDead;

	//UPROPERTY(EditAnywhere, Category = Info)
	//FText Name = FText::FromString("Spider");
	//UPROPERTY(EditAnywhere, Category = Info)
	//int Level = 1;
	//UPROPERTY(EditAnywhere, Category = Info)
	//float ExpForKill = 500.0f;
	//UPROPERTY(EditAnywhere, Category = Info)
	//bool bDoesRespawn = true;
	//UPROPERTY(EditAnywhere, Category = Info)
	//float RespawnTime = 10;


	UPROPERTY(VisibleAnywhere, Category = AI)
	class UAIPerceptionComponent* AIPerceptionComp;

	class UAISenseConfig_Sight* SightConfig;

	UPROPERTY(VisibleAnywhere, Category = UI)
	class UWidgetComponent* EnemyWidgetComp;
	UPROPERTY(VisibleAnywhere, Category = UI)
	class USphereComponent* ShowUICollision;

	class UEnemyInfoWidget* EnemyInfoWidget;


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

	void AdjustZToGround();
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnSightPerceptionUpdate(const TArray<AActor*>& UpdatedActors);

public:
	//UPROPERTY(VisibleAnywhere, Category = "Info")
	FMonsterInfo Info;

	UPROPERTY(EditAnywhere, Category = Info)
	int MonsterID = 0;

	FVector StartLocation;//初始的位置
	UPROPERTY(VisibleAnywhere, Category = Hit)
	class UArrowComponent* HitArrow;

	float CurrentHealth;

	UPROPERTY(EditAnywhere, Category = Behavior)
	TArray<class UAnimMontage*> AttackAnimaions;

	FORCEINLINE bool GetBDead() { return Info.bDead; }

	void UpdateHealthBar();
};

