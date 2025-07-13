//Copyright (C) RenZhai.2022.All Rights Reserved.
#include "AnimNotifyState/AnimNotifyState_ContinuousSpell.h"
#include "SimpleComboType.h"
#include "CombatInterface/SimpleCombatInterface.h"
#include "GameFramework/Character.h"

void UAnimNotifyState_ContinuousSpell::NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (MeshComp->GetOuter() && MeshComp->GetOuter()->GetWorld())
	{
		if (ISimpleCombatInterface* InCharacter = Cast<ISimpleCombatInterface>(MeshComp->GetOuter()))
		{
			if (FContinuousReleaseSpell *InSpell = InCharacter->GetContinuousReleaseSpell())
			{
				if (InSpell->ContinuousReleaseSpellIndex == 0 ||
					InSpell->ContinuousReleaseSpellIndex == INDEX_NONE)
				{
					//进入下一个循环
					InSpell->ContinuousReleaseSpellIndex = 1;
				}
			}
		}	
	}
}

void UAnimNotifyState_ContinuousSpell::NotifyTick(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime,EventReference);


}

void UAnimNotifyState_ContinuousSpell::NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation,EventReference);

	if (MeshComp->GetOuter() && MeshComp->GetOuter()->GetWorld())
	{	
		if (ISimpleCombatInterface* InCharacterInterface = Cast<ISimpleCombatInterface>(MeshComp->GetOuter()))
		{
			if (FContinuousReleaseSpell* InSpell = InCharacterInterface->GetContinuousReleaseSpell())
			{	
				if (InSpell->AnimMontage)
				{
					FName SectionName = *FString::FromInt(InSpell->ContinuousReleaseSpellIndex);
					if (ACharacter* InCharacter = Cast<ACharacter>(MeshComp->GetOuter()))
					{
						if (InSpell->ContinuousReleaseSpellIndex == 2)
						{
							InCharacter->PlayAnimMontage(
								InSpell->AnimMontage, 1.f,
								SectionName);
						}
						else
						{
							if (InCharacter->GetMesh())
							{
								if (UAnimInstance* InAnimInstance = InCharacter->GetMesh()->GetAnimInstance())
								{
									InAnimInstance->Montage_JumpToSection(SectionName);
									InAnimInstance->Montage_SetNextSection(SectionName, SectionName);
								}
							}
						}
					}
				}
			}
		}	
	}
}