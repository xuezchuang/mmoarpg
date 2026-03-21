// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../../../../DataTable/CharacterAnimTable.h"
//#include "CombatInterface/SimpleCombatInterface.h"
#include "../../../../MMOARPGGameType.h"
#include "Interface/DamageableInterface.h"
#include "MMOARPGCharacterBase.generated.h"

class UFlyComponent;
class UCameraComponent;
class UInteractionComponent;

UCLASS()
class MMOARPG_API AMMOARPGCharacterBase : public ACharacter//, public IDamageableInterface//,public ISimpleCombatInterface
{
	GENERATED_BODY()

	friend class AMMOARPGGameMode;

	UPROPERTY()
	TObjectPtr<UFlyComponent> FlyComponent;

	int a = 3;
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	UInteractionComponent* InteractionComp;

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void SetInteractionComponent(UInteractionComponent* NewComponent);

	void Interaction();
	void StopInteractionHold();
	// Sets default values for this character's properties
	AMMOARPGCharacterBase(const FObjectInitializer& ObjectInitializer);

	virtual void AnimSignal(int32 InSignal);
	virtual UCameraComponent* GetFollowCamera() const { return NULL; }

	//virtual void OnReceiveDamage(float BaseDamage, EDamageType Type, TSubclassOf<ABaseElement> Element, int CritChance, AActor* Attacker, ABaseSkill* Spell) override;

public:
	UFUNCTION(BlueprintCallable,BlueprintImplementableEvent, DisplayName = "AnimSignal", Category = "Anim Event")
	void K2_AnimSignal(int32 InSignal);

	UFUNCTION(BlueprintCallable, Category = "Anim Event")
	float GetSpeed();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void ResetActionState(ECharacterActionState InNewActionState);

	FORCEINLINE ECharacterActionState GetActionState() { return ActionState; }
	FORCEINLINE FCharacterAnimTable *GetAnimTable() { return AnimTable; }
	FORCEINLINE int32 GetID() { return ID; }
	FORCEINLINE int32 GetUserID() { return UserID; }
	FORCEINLINE UFlyComponent *GetFlyComponent() { return FlyComponent; }


protected:
	UFUNCTION(Server,Reliable)
	void SwitchActionStateOnServer(ECharacterActionState InActionState);
protected:
	UPROPERTY(ReplicatedUsing = OnRep_ActionStateChanged)
	ECharacterActionState ActionState;

	UPROPERTY()
	ECharacterActionState LastActionState;

	UFUNCTION()
	virtual void OnRep_ActionStateChanged();

	UPROPERTY(EditDefaultsOnly, Category = "Character")
	int32 ID;

	UPROPERTY()
	int32 UserID;
protected:
	FCharacterAnimTable *AnimTable;

	float m_CurSpeed;
};
