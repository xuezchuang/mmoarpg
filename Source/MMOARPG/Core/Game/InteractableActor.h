// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/MMOARPGCharacterBase.h"
#include "InteractableActor.generated.h"

class USelectableComponent;
class UInteractionComponent;
//class UWidgetComponent;

UCLASS()
class MMOARPG_API AInteractableActor : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	TSubclassOf<UInteractionComponent> InteractionComponentClass;


public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	UInteractionComponent* InteractionComp;

	//UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Interaction")
	//UWidgetComponent* InteractionWidget;

	AInteractableActor(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay()	override;
protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Selectable")
	USelectableComponent* SelectableComp;


	UFUNCTION(BlueprintNativeEvent, Category = "Selectable")
	void HandleSelected();

	UFUNCTION(BlueprintNativeEvent, Category = "Selectable")
	void HandleSelectionEnd();
};

