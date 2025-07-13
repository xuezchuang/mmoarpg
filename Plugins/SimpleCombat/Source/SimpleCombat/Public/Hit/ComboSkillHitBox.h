//Copyright (C) RenZhai.2022.All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Hit/Core/ComboSkillHitCollision.h"
#include "ComboSkillHitBox.generated.h"

UCLASS(BlueprintType, Blueprintable)
class SIMPLECOMBAT_API AHitBoxCollision : public AHitCollision
{
	GENERATED_BODY()

	//Åö×²ºÐ×Ó
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BaseAttrubute", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* HitDamage;
public:
	AHitBoxCollision(const FObjectInitializer& ObjectInitializer);

	virtual UPrimitiveComponent* GetHitDamage();

	void SetBoxExtent(const FVector& InNewBoxExtent);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;
};