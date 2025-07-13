//Copyright (C) RenZhai.2022.All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Hit/Core/ComboSkillHitCollision.h"
#include "ComboSkillHitCapsule.generated.h"

UCLASS(BlueprintType, Blueprintable)
class SIMPLECOMBAT_API AHitCapsuleCollision : public AHitCollision
{
	GENERATED_BODY()

	//Åö×²ºÐ×Ó
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BaseAttrubute", meta = (AllowPrivateAccess = "true"))
	class UCapsuleComponent* HitDamage;

public:
	AHitCapsuleCollision(const FObjectInitializer& ObjectInitializer);

	virtual UPrimitiveComponent* GetHitDamage();

	void SetCapsuleHalfHeight(float InNewCapsuleHalfHeight);
	void SetCapsuleRadius(float InNewCapsuleRadius);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;
};