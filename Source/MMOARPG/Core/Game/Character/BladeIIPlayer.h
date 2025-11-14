#pragma once

#include "CoreMinimal.h"
#include "Core/MMOARPGCharacterBase.h"
#include "InputActionValue.h"
#include "Interface/SelectableInterface.h"
#include "BladeIIPlayer.generated.h"

class FSimpleChannel;
struct S_MOVE_ROLE;
class USelectableComponent;

UCLASS(config=Game)
class ABladeIIPlayer : public AMMOARPGCharacterBase//,public ISelectableInterface
{
	GENERATED_BODY()

public:
	ABladeIIPlayer();

	virtual void BeginPlay()	override;

protected:
 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Selectable")
    USelectableComponent* SelectableComp;


	UFUNCTION(BlueprintNativeEvent, Category="Selectable")
	void HandleSelected();

	UFUNCTION(BlueprintNativeEvent, Category="Selectable")
	void HandleSelectionEnd();
};

