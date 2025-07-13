// Fill out your copyright notice in the Description page of Project Settings.
#include "InventoryHUD.h"
#include "UI_UpperUIBar.h"

AInventoryHUD::AInventoryHUD()
{
	static ConstructorHelpers::FClassFinder<UUI_UpperUIBar> BPClass(TEXT("/Game/MetanoiaCombat/Inventory/UI/WB_UpperUIBar"));
	UpperUIBarClass = BPClass.Class;
}

UUI_UpperUIBar* AInventoryHUD::GetWidget()
{
	return UpperUIBar;
}

void AInventoryHUD::BeginPlay()
{
	Super::BeginPlay();
	//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, FString::Printf(TEXT("Test")));
	if (UpperUIBarClass)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, FString::Printf(TEXT("Test")));
		UpperUIBar = CreateWidget<UUI_UpperUIBar>(GetWorld(), UpperUIBarClass);
		UpperUIBar->AddToViewport();
		UpperUIBar->SetVisibility(ESlateVisibility::Hidden);
		APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
		//if (PlayerController)
		//{
		//	FInputModeGameOnly InputMode;
		//	PlayerController->SetInputMode(InputMode);
		//}
	}
}
