// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/MMOARPGCharacterBase.h"
#include "MMOARPGNPC.generated.h"

class USelectableComponent;
class UInteractionComponent;
//class UWidgetComponent;
/**
 * 
 */
UCLASS()
class MMOARPG_API AMMOARPGNPC : public AMMOARPGCharacterBase
{
	GENERATED_BODY()

public:


	//UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Interaction")
	//UWidgetComponent* InteractionWidget;
public:
	AMMOARPGNPC(const FObjectInitializer& ObjectInitializer);
	
	virtual void BeginPlay()	override;
protected:

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	//UInteractionComponent* InteractionComp;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	//UInteractionComponent* InteractionComponent;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction", meta = (AllowAbstract = false))
	//TSubclassOf<UInteractionComponent> InteractionComponentClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Selectable")
	USelectableComponent* SelectableComp;

	UFUNCTION(BlueprintNativeEvent, Category = "Selectable")
	void HandleSelected();

	UFUNCTION(BlueprintNativeEvent, Category = "Selectable")
	void HandleSelectionEnd();
};
