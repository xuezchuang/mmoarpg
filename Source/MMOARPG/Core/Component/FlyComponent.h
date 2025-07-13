// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../../MMOARPGGameType.h"
#include "FlyComponent.generated.h"

class AMMOARPGCharacterBase;
class UCharacterMovementComponent;
class UCapsuleComponent;
class UCameraComponent;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MMOARPG_API UFlyComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimAttrubute")
	FVector2D RotationRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimAttrubute")
	EDodgeFly DodgeFly;

	FResetBool bFastFly;
	FResetBool bLand;
public:	
	// Sets default values for this component's properties
	UFlyComponent();
public:
	void FlyForwardAxis(float InAxisValue);
	void ResetFastFly();
	void ResetDodgeFly(EDodgeFly InFlyState);
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void Print(float InTime,const FString &InString);

	void Reset();
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void ResetFly();

	UFUNCTION()
	void Landed(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
protected:
	UPROPERTY()
	TWeakObjectPtr<AMMOARPGCharacterBase> MMOARPGCharacterBase;

	UPROPERTY()
	TWeakObjectPtr<UCapsuleComponent> CapsuleComponent;

	UPROPERTY()
	TWeakObjectPtr<UCameraComponent> CameraComponent;

	UPROPERTY()
	TWeakObjectPtr<UCharacterMovementComponent> CharacterMovementComponent;

	UPROPERTY()
	FRotator LastRotator;
};
