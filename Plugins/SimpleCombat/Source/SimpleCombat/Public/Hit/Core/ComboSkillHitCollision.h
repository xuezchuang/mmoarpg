//Copyright (C) RenZhai.2022.All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "SimpleComboType.h"
#include "ComboSkillHitCollision.generated.h"

class USplineComponent;
class UParticleSystem;
class UNiagaraSystem;

UCLASS(BlueprintType, Blueprintable)
class SIMPLECOMBAT_API AHitCollision : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HitCollision", meta = (AllowPrivateAccess = "true"))
	class USceneComponent* HitCollisionRootComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HitCollision", meta = (AllowPrivateAccess = "true"))
	class UProjectileMovementComponent* ProjectileMovement;
public:
	UPROPERTY(EditDefaultsOnly, Category = "HitCollision")
	EHitCollisionType HitCollisionType;
	
	//开火特效
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitCollision")
	UNiagaraSystem* OpenFireNiagara;

	//开火特效
	UPROPERTY(EditDefaultsOnly, Category = "HitCollision")
	UParticleSystem* OpenFireParticle;

	UPROPERTY(EditDefaultsOnly, Category = "HitCollision")
	FName OpenFirePoint;

	UPROPERTY(EditDefaultsOnly, Category = "HitCollision")
	bool bRandomDirection;
public:
	AHitCollision(const FObjectInitializer& ObjectInitializer);

	virtual void PreInitCollision(AActor* InMyInstigator);

	UFUNCTION()
	virtual void HandleDamage(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void SetHitDamageRelativePosition(const FVector &InNewPostion);

	virtual UPrimitiveComponent* GetHitDamage();

	void SetHitID(int32 InNewHit) { HitID = InNewHit; }
	void SetBuffs(const TArray<FName> &InBuffs) { Buffs = InBuffs; }
	void SetBuffs(const TArray<FGameplayTag>& InBuffs) { BuffTags = InBuffs; }
	const int32 GetHitID() const { return HitID; }

	void Collision(bool bCollision);
	bool IsExist(ACharacter *InNewTaget)const;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY()
	USplineComponent* Spline;

	UPROPERTY()
	float SplineOffset;

	UPROPERTY()
	int32 HitID;

	UPROPERTY()
	TArray<FName> Buffs;

	UPROPERTY()
	TArray<FGameplayTag> BuffTags;

	UPROPERTY()
	TArray<TWeakObjectPtr<ACharacter>> AttackedTarget;

	UPROPERTY()
	float CurrentSplineTime;

	UPROPERTY()
	FVector DistanceVector;
};