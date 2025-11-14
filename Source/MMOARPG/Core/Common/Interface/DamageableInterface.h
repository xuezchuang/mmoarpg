// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DamageableInterface.generated.h"

class ABaseElement;
class ABaseSkill;

//伤害类型
UENUM()
enum class EDamageType :uint8 
{
	Magic,
	Physical
};

UINTERFACE()
class UDamageableInterface :public UInterface {
	GENERATED_BODY()
};

class IDamageableInterface {
	GENERATED_BODY()
public:
	virtual void OnReceiveDamage(float BaseDamage,EDamageType Type,TSubclassOf<ABaseElement> Element,int CritChance,AActor* Attacker,ABaseSkill* Spell) = 0;
};