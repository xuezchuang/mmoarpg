//Copyright (C) RenZhai.2022.All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Hit/Core/ComboSkillHitCollision.h"
#include "ComboSkillHitCustom.generated.h"

UCLASS(BlueprintType, Blueprintable)
class SIMPLECOMBAT_API AHitCustiomCollision : public AHitCollision
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;
};