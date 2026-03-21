// Fill out your copyright notice in the Description page of Project Settings.


#include "MMOARPGNPC.h"
#include "Component/SelectableComponent.h"
#include "Component/InteractionComponent.h"

AMMOARPGNPC::AMMOARPGNPC(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	SelectableComp = CreateDefaultSubobject<USelectableComponent>(TEXT("SelectableComp"));

	InteractionComp = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));
}

void AMMOARPGNPC::BeginPlay()
{
	//if (InteractionComponentClass)
	//{
	//	InteractionComponent = NewObject<UInteractionComponent>(this, InteractionComponentClass);
	//	InteractionComponent->RegisterComponent();
	//}

	Super::BeginPlay();

	if (SelectableComp)
	{
		SelectableComp->OnSelectedEvent.AddDynamic(this, &AMMOARPGNPC::HandleSelected);
		SelectableComp->OnSelectionEndEvent.AddDynamic(this, &AMMOARPGNPC::HandleSelectionEnd);
	}


}

void AMMOARPGNPC::HandleSelected_Implementation()
{

}

void AMMOARPGNPC::HandleSelectionEnd_Implementation()
{

}
