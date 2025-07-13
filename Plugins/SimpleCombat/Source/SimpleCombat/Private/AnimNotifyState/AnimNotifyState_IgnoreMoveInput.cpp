//Copyright (C) RenZhai.2022.All Rights Reserved.

#include "AnimNotifyState/AnimNotifyState_IgnoreMoveInput.h"
#include "GameFramework/Character.h"

void UAnimNotifyState_IgnoreMoveInput::NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (ACharacter* InCharacter = Cast<ACharacter>(MeshComp->GetOuter()))
	{
		if (InCharacter->GetWorld())
		{
			if (InCharacter->GetWorld()->IsNetMode(ENetMode::NM_Client))
			{
				if (InCharacter->GetLocalRole() == ENetRole::ROLE_AutonomousProxy)
				{
					if (InCharacter->GetController())
					{
						InCharacter->GetController()->SetIgnoreMoveInput(true);
					}
				}
			}
			else if (InCharacter->GetWorld()->IsNetMode(ENetMode::NM_Standalone) ||
				InCharacter->GetWorld()->IsNetMode(ENetMode::NM_ListenServer))
			{
				if (InCharacter->GetController())
				{
					InCharacter->GetController()->SetIgnoreMoveInput(true);
				}
			}
		}
	}
}

void UAnimNotifyState_IgnoreMoveInput::NotifyTick(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime,EventReference);
}

void UAnimNotifyState_IgnoreMoveInput::NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation,EventReference);

	if (ACharacter* InCharacter = Cast<ACharacter>(MeshComp->GetOuter()))
	{
		if (InCharacter->GetWorld())
		{
			if (InCharacter->GetWorld()->IsNetMode(ENetMode::NM_Client))
			{
				if (InCharacter->GetLocalRole() == ENetRole::ROLE_AutonomousProxy)
				{
					if (InCharacter->GetController())
					{
						InCharacter->GetController()->SetIgnoreMoveInput(false);
					}
				}
			}
			else if (InCharacter->GetWorld()->IsNetMode(ENetMode::NM_Standalone) ||
				InCharacter->GetWorld()->IsNetMode(ENetMode::NM_ListenServer))
			{
				if (InCharacter->GetController())
				{
					InCharacter->GetController()->SetIgnoreMoveInput(false);
				}
			}
		}
	}
}