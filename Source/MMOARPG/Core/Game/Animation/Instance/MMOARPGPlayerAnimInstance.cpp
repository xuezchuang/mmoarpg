// Fill out your copyright notice in the Description page of Project Settings.

#include "MMOARPGPlayerAnimInstance.h"
#include "../../../Common/Interface/KneadingInterface.h"

void UMMOARPGPlayerAnimInstance::NativeUpdateAnimation(float Deltaseconds)
{
	Super::NativeUpdateAnimation(Deltaseconds);

	if (IKneadingInterface* InCharacterBase = Cast<IKneadingInterface>(TryGetPawnOwner()))
	{
		LegSize = InCharacterBase->GetLegSize();
		WaistSize = InCharacterBase->GetWaistSize();
		ArmSize = InCharacterBase->GetArmSize();
	}
}

