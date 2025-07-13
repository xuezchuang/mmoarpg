//Copyright (C) RenZhai.2022.All Rights Reserved.
#include "AnimNotify/AnimNotify_PlayGameplayEffect.h"
#include "CombatInterface/SimpleCombatInterface.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Script/CustomReleasingBuffTarget.h"

UAnimNotify_PlayGameplayEffect::UAnimNotify_PlayGameplayEffect()
	:bTarget(false)
{

}

FString UAnimNotify_PlayGameplayEffect::GetNotifyName_Implementation() const
{
	return Super::GetNotifyName_Implementation();
}

void UAnimNotify_PlayGameplayEffect::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (AActor* InOnwer = Cast<AActor>(MeshComp->GetOuter()))
	{
		if (InOnwer->GetWorld() && InOnwer->GetWorld()->IsNetMode(ENetMode::NM_DedicatedServer))
		{
			if (ISimpleCombatInterface* InSimpleCombatInterface = Cast<ISimpleCombatInterface>(MeshComp->GetOuter()))
			{
				TArray<AActor*> FindTargets;
				if (CustomReleasingBuffTarget)
				{
					if (UCustomReleasingBuffTarget* InTarget = Cast<UCustomReleasingBuffTarget>(CustomReleasingBuffTarget->GetDefaultObject()))
					{
						InTarget->GetCustomReleasingBuffTarget(InOnwer, FindTargets);
					}
				}
				else if (bTarget)
				{
					FindTargets.Add(InSimpleCombatInterface->GetTarget());
				}
				else
				{
					FindTargets.Add(InOnwer);
				}

				for (auto& Tmp : FindTargets)
				{
					check(Tmp != NULL);

					if (IAbilitySystemInterface* InAbilitySystemInterface = Cast<IAbilitySystemInterface>(Tmp))
					{
						if (UAbilitySystemComponent* InAbilitySystemComponent = InAbilitySystemInterface->GetAbilitySystemComponent())
						{
							FGameplayEffectContextHandle EffectContext = InAbilitySystemComponent->MakeEffectContext();

							EffectContext.AddSourceObject(Tmp);

							FGameplayEffectSpecHandle EffectSpecHandle =
								InAbilitySystemComponent->MakeOutgoingSpec(EffectClass,
									InSimpleCombatInterface->GetCharacterLevel(),
									EffectContext);

							if (EffectSpecHandle.IsValid())
							{
								InAbilitySystemComponent->ApplyGameplayEffectSpecToTarget(
									*EffectSpecHandle.Data.Get(),
									InAbilitySystemComponent);
							}
						}
					}
				}
			}
		}
	}
}
