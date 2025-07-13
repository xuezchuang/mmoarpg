//Copyright (C) RenZhai.2022.All Rights Reserved.
#include "AnimNotifyState/AnimNotifyState_IgnoreInput.h"
#include "GameFramework/Character.h"

void UAnimNotifyState_IgnoreInput::NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (ACharacter* InCharacter = Cast<ACharacter>(MeshComp->GetOuter()))
	{
		if (InCharacter->GetWorld()->IsNetMode(ENetMode::NM_Client))
		{
			if (InCharacter->GetLocalRole() == ENetRole::ROLE_AutonomousProxy)
			{
				InCharacter->DisableInput(InCharacter->GetWorld()->GetFirstPlayerController());
			}
		}
		else if (InCharacter->GetWorld()->IsNetMode(ENetMode::NM_Standalone) || 
			InCharacter->GetWorld()->IsNetMode(ENetMode::NM_ListenServer))
		{
			InCharacter->DisableInput(InCharacter->GetWorld()->GetFirstPlayerController());
		}
	}
}

void UAnimNotifyState_IgnoreInput::NotifyTick(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime,EventReference);
}

void UAnimNotifyState_IgnoreInput::NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation,EventReference);

	if (ACharacter* InCharacter = Cast<ACharacter>(MeshComp->GetOuter()))
	{
		if (InCharacter->GetWorld()->IsNetMode(ENetMode::NM_Client))
		{
			if (InCharacter->GetLocalRole() == ENetRole::ROLE_AutonomousProxy)
			{
				InCharacter->EnableInput(InCharacter->GetWorld()->GetFirstPlayerController());
			}
		}
		else if (InCharacter->GetWorld()->IsNetMode(ENetMode::NM_Standalone) || 
				InCharacter->GetWorld()->IsNetMode(ENetMode::NM_ListenServer))
		{
			InCharacter->EnableInput(InCharacter->GetWorld()->GetFirstPlayerController());
		}
	}
}