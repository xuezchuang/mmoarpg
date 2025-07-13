//Copyright (C) RenZhai.2022.All Rights Reserved.
#include "AnimNotifyState/AnimNotifyState_StopSpeed.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UAnimNotifyState_StopSpeed::UAnimNotifyState_StopSpeed()
	:MaxWalkSpeed(0.f)
{

}

void UAnimNotifyState_StopSpeed::NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (ACharacter* InCharacter = Cast<ACharacter>(MeshComp->GetOuter()))
	{
		//´æ´¢Ò»·Ý
		MaxWalkSpeed = InCharacter->GetCharacterMovement()->MaxWalkSpeed;

		InCharacter->GetCharacterMovement()->MaxWalkSpeed = 0.f;
	}
}

void UAnimNotifyState_StopSpeed::NotifyTick(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime,EventReference);

}

void UAnimNotifyState_StopSpeed::NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation,EventReference);

	if (ACharacter* InCharacter = Cast<ACharacter>(MeshComp->GetOuter()))
	{
		InCharacter->GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
	}
}