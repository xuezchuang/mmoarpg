//Copyright (C) RenZhai.2022.All Rights Reserved.
#include "AnimNotifyState/AnimNotifyState_PlayTurnMontage.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UAnimNotifyState_PlayTurnMontage::UAnimNotifyState_PlayTurnMontage()
	:TurnMontage(NULL)
	,bPlayTurnMontage(true)
{

}

void UAnimNotifyState_PlayTurnMontage::NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (MeshComp->GetOuter() && MeshComp->GetOuter()->GetWorld())
	{
		if (ACharacter* InCharacter = Cast<ACharacter>(MeshComp->GetOuter()))
		{
			if (UCharacterMovementComponent* InCharacterMovementComponent = Cast<UCharacterMovementComponent>(InCharacter->GetCharacterMovement()))
			{
				LastInputVector = InCharacterMovementComponent->GetLastInputVector();
				bPlayTurnMontage = true;
			}
		}
	}
}

void UAnimNotifyState_PlayTurnMontage::NotifyTick(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime,EventReference);

	if (!TurnMontage)
	{
		return;
	}

	if (bPlayTurnMontage)
	{
		if (MeshComp->GetOuter() && 
			MeshComp->GetOuter()->GetWorld())
		{
			if (ACharacter* InCharacter = Cast<ACharacter>(MeshComp->GetOuter()))
			{
				if (UCharacterMovementComponent *InCharacterMovementComponent = Cast<UCharacterMovementComponent>(InCharacter->GetCharacterMovement()))
				{
					FVector NewLastInputVector = InCharacterMovementComponent->GetLastInputVector();
					if (!NewLastInputVector.IsZero())
					{
						if (FVector::DotProduct(NewLastInputVector, LastInputVector) < 0.f)
						{
							bPlayTurnMontage = false;
							InCharacter->PlayAnimMontage(TurnMontage);
						}

						LastInputVector = NewLastInputVector;
					}
				}	
			}
		}
	}
}

void UAnimNotifyState_PlayTurnMontage::NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation,EventReference);
}