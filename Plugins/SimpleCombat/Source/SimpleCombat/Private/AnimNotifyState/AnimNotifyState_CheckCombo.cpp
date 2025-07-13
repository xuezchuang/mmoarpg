//Copyright (C) RenZhai.2022.All Rights Reserved.
#include "AnimNotifyState/AnimNotifyState_CheckCombo.h"
#include "SimpleComboType.h"
#include "CombatInterface/SimpleCombatInterface.h"

void UAnimNotifyState_CheckCombo::NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (MeshComp->GetOuter() && MeshComp->GetOuter()->GetWorld())
	{
		if (MeshComp->GetOuter()->GetWorld()->IsNetMode(ENetMode::NM_DedicatedServer))
		{
			if (ISimpleCombatInterface* InCharacter = Cast<ISimpleCombatInterface>(MeshComp->GetOuter()))
			{
				InCharacter->GetSimpleComboInfo(ComboKey)->bShortPress = false;
				InCharacter->GetSimpleComboInfo(ComboKey)->UpdateComboIndex();
			}
		}
	}
}

void UAnimNotifyState_CheckCombo::NotifyTick(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime,EventReference);


}

void UAnimNotifyState_CheckCombo::NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation,EventReference);

	if (MeshComp->GetOuter() && MeshComp->GetOuter()->GetWorld())
	{
		if (MeshComp->GetOuter()->GetWorld()->IsNetMode(ENetMode::NM_DedicatedServer))
		{
			if (ISimpleCombatInterface* InCharacter = Cast<ISimpleCombatInterface>(MeshComp->GetOuter()))
			{
				if ((InCharacter->GetSimpleComboInfo(ComboKey)->bLongPress || InCharacter->GetSimpleComboInfo(ComboKey)->bShortPress))
				{
					InCharacter->ComboAttack(ComboKey);
				}
			}
		}
	}
}