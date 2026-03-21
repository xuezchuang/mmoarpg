// Fill out your copyright notice in the Description page of Project Settings.
#include "MMOARPGCharacterBase.h"
#include "../../MMOARPGGameState.h"
#include "Net/UnrealNetwork.h"
#include "../../Animation/Instance/Core/MMOARPGAnimInstanceBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Component/FlyComponent.h"
#include "Component/InteractionComponent.h"

void AMMOARPGCharacterBase::SetInteractionComponent(UInteractionComponent* NewComponent)
{
	InteractionComp = NewComponent;
}

void AMMOARPGCharacterBase::Interaction()
{
	if (InteractionComp == NULL)
		return;
	InteractionComp->StartHold();
}

void AMMOARPGCharacterBase::StopInteractionHold()
{
	if (InteractionComp)
	{
		InteractionComp->StopHold();
	}
}

// Sets default values
AMMOARPGCharacterBase::AMMOARPGCharacterBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
	,ActionState(ECharacterActionState::NORMAL_STATE)
	,LastActionState(ECharacterActionState::NORMAL_STATE)
	,ID(INDEX_NONE)
	,UserID(INDEX_NONE)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	FlyComponent = CreateDefaultSubobject<UFlyComponent>(TEXT("FlightComponent"));
//	FlyComponent->SetupAttachment(RootComponent);

	
	
}

// Called when the game starts or when spawned
void AMMOARPGCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::Printf(TEXT("Num:%d"), 333));
	//if (GetWorld())
	//{
	//	if (AMMOARPGGameState* InGameState = GetWorld()->GetGameState<AMMOARPGGameState>())
	//	{
	//		if (FCharacterAnimTable* InAnimTable = InGameState->GetCharacterAnimTable(GetID()))
	//		{
	//			AnimTable = InAnimTable;
	//		}
	//	}

	//	if (!GetWorld()->IsServer())
	//	{
	//		if (GetMesh())
	//		{
	//			if (UMMOARPGAnimInstanceBase* InMMOARPGAnimInstanceBase = Cast<UMMOARPGAnimInstanceBase>(GetMesh()->GetAnimInstance()))
	//			{
	//				InMMOARPGAnimInstanceBase->InitAnimInstance(this);
	//			}
	//		}
	//	}
	//}	
}

// Called every frame
void AMMOARPGCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMMOARPGCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AMMOARPGCharacterBase::SwitchActionStateOnServer_Implementation(ECharacterActionState InActionState)
{
	ActionState = InActionState;

	LastActionState = ActionState;
}

void AMMOARPGCharacterBase::ResetActionState(ECharacterActionState InNewActionState)
{
	//客户端
	if (ActionState == InNewActionState)
	{
		ActionState = ECharacterActionState::NORMAL_STATE;
	}
	else
	{
		ActionState = InNewActionState;
	}
}

void AMMOARPGCharacterBase::OnRep_ActionStateChanged()
{
}

void AMMOARPGCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AMMOARPGCharacterBase, ActionState, COND_SimulatedOnly);
}

void AMMOARPGCharacterBase::AnimSignal(int32 InSignal)
{
	K2_AnimSignal(InSignal);
}

//void AMMOARPGCharacterBase::OnReceiveDamage(float BaseDamage, EDamageType Type, TSubclassOf<ABaseElement> Element, int CritChance, AActor* Attacker, ABaseSkill* Spell)
//{
//	//throw std::logic_error("The method or operation is not implemented.");
//}

float AMMOARPGCharacterBase::GetSpeed()
{
	return m_CurSpeed;
}
