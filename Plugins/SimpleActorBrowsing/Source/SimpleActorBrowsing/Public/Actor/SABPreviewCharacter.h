//Copyright (C) RenZhai.2022.All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SABPreviewCharacter.generated.h"

UCLASS()
class SIMPLEACTORBROWSING_API ASABPreviewCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	void SetSABObject(UObject*InSAB);

	UFUNCTION()
	virtual void OnClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed);

protected:
	virtual void BeginPlay() override;

private:
	TWeakObjectPtr<UObject> SABInterface;
};