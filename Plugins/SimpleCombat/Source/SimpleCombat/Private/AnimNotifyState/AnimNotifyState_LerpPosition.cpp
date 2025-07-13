//Copyright (C) RenZhai.2022.All Rights Reserved.
#include "AnimNotifyState/AnimNotifyState_LerpPosition.h"
#include "GameFramework/Character.h"
#include "SimpleComboType.h"

UAnimNotifyState_LerpPosition::UAnimNotifyState_LerpPosition()
	:Super()
{
	TotalDurationConsuming = 0.f;
	Direction = FVector(1.0f, 0.f, 0.f);
	bAbsoluteCoordinates = false;
	Distance = 50.f;
	EndPositionInterval = FVector::ZeroVector;
	bPlayOnlyServer = false;
}

void UAnimNotifyState_LerpPosition::NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	TotalDurationConsuming = TotalDuration;

	if (ACharacter* InCharacter = Cast<ACharacter>(MeshComp->GetOuter()))
	{
		FVector NewDirection = FVector::ZeroVector;
		if (bAbsoluteCoordinates)
		{
			NewDirection = Direction;
		}
		else
		{
			NewDirection = SimpleComboType::GetCurrentCharacterDirection(InCharacter, Direction);
		}

		EndPositionInterval = NewDirection * Distance;
	}
}

void UAnimNotifyState_LerpPosition::NotifyTick(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime,EventReference);

	if (TotalDurationConsuming > 0.f)
	{
		float FrameDeltaOffset = FrameDeltaTime / TotalDurationConsuming;
		if (ACharacter* InCharacter = Cast<ACharacter>(MeshComp->GetOuter()))
		{
			if (bPlayOnlyServer)
			{
				if (!InCharacter->GetWorld()->IsNetMode(ENetMode::NM_DedicatedServer))
				{
					return;
				}
			}

			FVector NewPosition = InCharacter->GetActorLocation() + EndPositionInterval * FrameDeltaOffset;;
			
			InCharacter->SetActorLocation(NewPosition);
		}
	}
}

void UAnimNotifyState_LerpPosition::NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation,EventReference);

}
