// Fill out your copyright notice in the Description page of Project Settings.

#include "InventoryComponent.h"
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

void UInventoryComponent::InitializeComponent()
{
	/*m_InventoryGameState = GetWorld()->GetGameState<AInventoryGameState>();*/
}

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	if(GetWorld())
		m_InventoryGameState = GetWorld()->GetGameState<AInventoryGameState>();
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	// ...
}

void UInventoryComponent::ShowVendor()
{
	if(m_CanShop && m_UIShop)
	{
		m_UIShop->SetVisibility(ESlateVisibility::Visible);
		UMMOARPGBPLibrary::MMOARPG_InputMode(GetWorld(), EMMOARPG_InputMode::Input_UI, m_UIShop);
	}
}

// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	
	MMOARPGCharacterBase = Cast<AMMOARPGCharacterBase>(GetOwner());
	if (MMOARPGCharacterBase.IsValid())
	{
		m_UIShop = CreateWidget<UUI_Vendor>(GetWorld(), Vendor_BPClass);
		m_UIShop->AddToViewport();
		m_UIShop->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UInventoryComponent::Print(float InTime,const FString& InString)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, InTime, FColor::Red, FString::Printf(TEXT("%s"), *InString));
	}
}

void UInventoryComponent::Reset()
{
	//CharacterMovementComponent->bOrientRotationToMovement = true;
	//CharacterMovementComponent->MaxFlySpeed = 600.f;
	//CharacterMovementComponent->SetMovementMode(EMovementMode::MOVE_Walking);

	//FRotator NewRot = MMOARPGCharacterBase->GetActorRotation();
	//NewRot.Pitch = 0.f;
	//MMOARPGCharacterBase->SetActorRotation(NewRot);
}

// Called every frame
void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

FSlateColor UInventoryComponent::GetRarityColor(E_ItemRarity rarity)
{
	if(!m_InventoryGameState)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, TEXT("m_InventoryGameState = NULL"));
		return FSlateColor();
	}
	return m_InventoryGameState->GetRarityColor(rarity);
}

UTexture2D* UInventoryComponent::GetStatTexture(E_StatCategory stat)
{
	return *mapStatTexture.Find(stat);
}

UInventoryComponent* UInventoryComponent::GetInventoryComponent(const UObject* WorldContextObject)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if(!World)
	{
		return nullptr;
	}

	APlayerController* PC = World->GetFirstPlayerController<APlayerController>();
	ACharacter* Pawn = Cast<ACharacter>(PC->GetPawn());
	return Pawn->FindComponentByClass<UInventoryComponent>();
}

