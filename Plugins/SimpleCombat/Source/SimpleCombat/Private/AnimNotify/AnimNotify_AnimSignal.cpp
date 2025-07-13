//Copyright (C) RenZhai.2022.All Rights Reserved.
#include "AnimNotify/AnimNotify_AnimSignal.h"
#include "CombatInterface/SimpleCombatInterface.h"

UAnimNotify_AnimSignal::UAnimNotify_AnimSignal()
	:SignalValue(INDEX_NONE)
{

}

FString UAnimNotify_AnimSignal::GetNotifyName_Implementation() const
{
	return Super::GetNotifyName_Implementation();
}

void UAnimNotify_AnimSignal::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (ISimpleCombatInterface *InSimpleCombatInterface = Cast<ISimpleCombatInterface>(MeshComp->GetOuter()))
	{
		InSimpleCombatInterface->AnimSignal(SignalValue);
	}
}