// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractableActor.h"
#include "Component/SelectableComponent.h"
#include "Component/InteractionComponent.h"

AInteractableActor::AInteractableActor(const FObjectInitializer& ObjectInitializer)
{
	SelectableComp = CreateDefaultSubobject<USelectableComponent>(TEXT("SelectableComp"));

	InteractionComp = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));
}

void AInteractableActor::BeginPlay()
{
	Super::BeginPlay();

	if (SelectableComp)
	{
		SelectableComp->OnSelectedEvent.AddDynamic(this, &AInteractableActor::HandleSelected);
		SelectableComp->OnSelectionEndEvent.AddDynamic(this, &AInteractableActor::HandleSelectionEnd);
	}
}

void AInteractableActor::HandleSelected_Implementation()
{

}

void AInteractableActor::HandleSelectionEnd_Implementation()
{

}

