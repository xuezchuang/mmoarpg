//Copyright (C) RenZhai.2022.All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SimpleOceanInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, Blueprintable, BlueprintType, meta = (CannotImplementInterfaceInBlueprint))
class USimpleOceanInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class SIMPLEOCEAN_API ISimpleOceanInterface
{
	GENERATED_BODY()

		// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(Blueprintable, Category = "Combat")
	virtual void OceanObjectEnter(AActor* InActor) {}

	UFUNCTION(Blueprintable, Category = "Combat")
	virtual void OceanObjectLeave(AActor* InActor) {}
};


