//Copyright (C) RenZhai.2022.All Rights Reserved.
#include "AnimNotifyState/AnimNotifyState_CheckAndStopMontage.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UAnimNotifyState_CASMontage::UAnimNotifyState_CASMontage()
{

}

void UAnimNotifyState_CASMontage::NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (MeshComp->GetOuter() && 
		MeshComp->GetOuter()->GetWorld())
	{
		if (ACharacter* InCharacter = Cast<ACharacter>(MeshComp->GetOuter()))
		{
			if (UCharacterMovementComponent* InCharacterMovementComponent = Cast<UCharacterMovementComponent>(InCharacter->GetCharacterMovement()))
			{
				LastInputVector = InCharacterMovementComponent->GetLastInputVector();
			}
		}
	}
}

void UAnimNotifyState_CASMontage::NotifyTick(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime,EventReference);

	if (MeshComp->GetOuter() && 
		MeshComp->GetOuter()->GetWorld())
	{
		if (ACharacter* InCharacter = Cast<ACharacter>(MeshComp->GetOuter()))
		{
			if (UCharacterMovementComponent *InCharacterMovementComponent = Cast<UCharacterMovementComponent>(InCharacter->GetCharacterMovement()))
			{
				FVector NewLastInputVector = InCharacterMovementComponent->GetLastInputVector();
				//if (!NewLastInputVector.IsZero())
				{
					if (NewLastInputVector != LastInputVector)
					{
						InCharacter->StopAnimMontage();
					}
				}
			}	
		}
	}
}

void UAnimNotifyState_CASMontage::NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation,EventReference);
}