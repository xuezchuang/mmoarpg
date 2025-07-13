//Copyright (C) RenZhai.2022.All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "SimpleCombatBPLibrary.generated.h"

class UUI_ComboCount;
class AResidualShadowActor;

UCLASS()
class SIMPLECOMBAT_API USimpleCombatBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

public:
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "CombatText", Category = "Combat", WorldContext = WorldContextObject))
	static void ComboPlay(UObject* WorldContextObject,TSubclassOf<UUI_ComboCount> InClass);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "CombatText", Category = "Combat"))
	static void ComboTextDestroy();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "ResidualShadow", Category = "Combat", WorldContext = WorldContextObject))
	static AResidualShadowActor *SpawnResidualShadow(
		UObject* WorldContextObject,
		TSubclassOf<AResidualShadowActor>ResidualShadowActorClass,
		USkeletalMeshComponent* InComponentToCopy,
		float InZOffset,
		const FVector &Location,const FRotator &InRot,float InLiftTime = 1.f);
};
