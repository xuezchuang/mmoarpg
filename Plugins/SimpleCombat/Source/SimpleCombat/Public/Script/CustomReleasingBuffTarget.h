//Copyright (C) RenZhai.2022.All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CustomReleasingBuffTarget.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class SIMPLECOMBAT_API UCustomReleasingBuffTarget : public UObject
{
	GENERATED_BODY()
	
public:
	virtual void GetCustomReleasingBuffTarget(AActor *InOwner,TArray<AActor*> &OutTarget);

	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "CustomReleasing", meta = (DisplayName = "GetCustomReleasingBuffTarget"))
	void K2_GetCustomReleasingBuffTarget(AActor* InOwner, TArray<AActor*>& OutTarget);
	void K2_GetCustomReleasingBuffTarget_Implementation(AActor* InOwner, TArray<AActor*>& OutTarget) { };

	//UFUNCTION(BlueprintCallable,BlueprintPure, Category = "CustomReleasing")
	UFUNCTION(BlueprintCallable,Category = "CustomReleasing")
	UWorld* GetWorld2(AActor*InOwner);

	UFUNCTION(BlueprintCallable, Category = "CustomReleasing")
	bool GetAllActorsOfClass(AActor* InOwner, TSubclassOf<AActor> ActorClass,TArray<AActor*> &InArray);
};
