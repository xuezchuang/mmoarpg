//Copyright (C) RenZhai.2022.All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SimpleCombatInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, Blueprintable, BlueprintType, meta = (CannotImplementInterfaceInBlueprint))
class USimpleCombatInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SIMPLECOMBAT_API ISimpleCombatInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(Blueprintable, Category="Combat")
	virtual void AnimSignal(int32 InSignal) {}

	UFUNCTION(Blueprintable, Category = "Combat")
	virtual bool ComboAttack(const FName& InKey) { return false; }

	UFUNCTION(Blueprintable, Category = "Combat")
	virtual AActor* GetTarget() { return NULL; }

	UFUNCTION(Blueprintable, Category = "Combat")
	virtual float GetCharacterLevel() { return 1; }

	virtual struct FSimpleComboCheck* GetSimpleComboInfo(const FName &InKey) { return NULL; }

	virtual struct FContinuousReleaseSpell* GetContinuousReleaseSpell() { return NULL; }
};
