//Copyright (C) RenZhai.2022.All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "Camera/CameraModifier_CameraShake.h"
#include "ComboPlayerCameraManager.generated.h"

class UCameraShakeBase;
/**
 * 
 */
UCLASS()
class SIMPLECOMBAT_API AComboPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

public:
	virtual UCameraShakeBase* PlayCameraShake(
		TSubclassOf<UCameraShakeBase> InShakeClass,
		FOnInitializeCameraShake OnInitializeCameraShake = FOnInitializeCameraShake(),
		float Scale = 1.f,
		ECameraShakePlaySpace PlaySpace = ECameraShakePlaySpace::CameraLocal,
		FRotator UserPlaySpaceRot = FRotator::ZeroRotator);
};
