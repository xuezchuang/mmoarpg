#pragma once

#include "CoreMinimal.h"
#include "BladeIICharacter.h"
#include "InputActionValue.h"
#include "BladeIIPlayer.generated.h"

class FSimpleChannel;
struct S_MOVE_ROLE;

UCLASS(config=Game)
class ABladeIIPlayer : public ABladeIICharacter
{
	GENERATED_BODY()

public:
	ABladeIIPlayer();

	virtual void BeginPlay()	override;
};

