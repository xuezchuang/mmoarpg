// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractionComponent.h"
#include "../Game/Character/Core/MMOARPGCharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "MMOARPGType.h"
#include "../../Inventory/UI_Vendor.h"
#include "../../MMOARPGBPLibrary.h"
#include "../../Inventory/InventoryGameState.h"

void UInteractionComponent::InitializeComponent()
{
	/*m_InventoryGameState = GetWorld()->GetGameState<AInventoryGameState>();*/
}

// Sets default values for this component's properties
UInteractionComponent::UInteractionComponent()
{
	//if(GetWorld())
	//	m_InventoryGameState = GetWorld()->GetGameState<AInventoryGameState>();
	//// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	//// off to improve performance if you don't need them.
	//PrimaryComponentTick.bCanEverTick = true;
	// ...
}

// Called when the game starts
void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	//
	//MMOARPGCharacterBase = Cast<AMMOARPGCharacterBase>(GetOwner());
	//if (MMOARPGCharacterBase.IsValid())
	//{
	//	m_UIShop = CreateWidget<UUI_Vendor>(GetWorld(), Vendor_BPClass);
	//	m_UIShop->AddToViewport();
	//	m_UIShop->SetVisibility(ESlateVisibility::Hidden);
	//}
}

void UInteractionComponent::Print(float InTime,const FString& InString)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, InTime, FColor::Red, FString::Printf(TEXT("%s"), *InString));
	}
}


// Called every frame
void UInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

