//Copyright (C) RenZhai.2022.All Rights Reserved.
#include "AnimNotifyState/AnimNotifyState_EnableGravity.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void UAnimNotifyState_EnableGravity::NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (ACharacter* InCharacter = Cast<ACharacter>(MeshComp->GetOuter()))
	{
		if (InCharacter->GetWorld())
		{
			if (InCharacter->GetMovementComponent())
			{
				if (UCharacterMovementComponent* InCMC = Cast<UCharacterMovementComponent>(InCharacter->GetMovementComponent()))
				{
					InCMC->GravityScale = 0.f;
				}

				InCharacter->GetMovementComponent()->Velocity = FVector(0.f,0.f,0.f);
			}
		}
	}
}

void UAnimNotifyState_EnableGravity::NotifyTick(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime,EventReference);
}

void UAnimNotifyState_EnableGravity::NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation,EventReference);

	if (ACharacter* InCharacter = Cast<ACharacter>(MeshComp->GetOuter()))
	{
		if (InCharacter->GetWorld())
		{
			if (InCharacter->GetMovementComponent())
			{
				InCharacter->GetMovementComponent()->Velocity = FVector(0.f, 0.f, 1.f);

				if (UCharacterMovementComponent *InCMC= Cast<UCharacterMovementComponent>(InCharacter->GetMovementComponent()))
				{
					InCMC->GravityScale = 1.f;
				}
			}
		}
	}
}