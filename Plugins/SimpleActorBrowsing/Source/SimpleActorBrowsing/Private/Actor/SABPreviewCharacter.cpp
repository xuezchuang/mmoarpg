#include "Actor/SABPreviewCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Interface/SABInterface.h"

// Called when the game starts or when spawned
void ASABPreviewCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (GetCapsuleComponent())
	{
		GetCapsuleComponent()->SetCollisionProfileName(TEXT("BlockAll"));
		GetCapsuleComponent()->OnClicked.AddDynamic(this, &ASABPreviewCharacter::OnClicked);
	}
}

void ASABPreviewCharacter::SetSABObject(UObject* InSAB)
{
	SABInterface = InSAB;
}

void ASABPreviewCharacter::OnClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed)
{
	if(ISABInterface *InInterface = Cast<ISABInterface>(SABInterface.Get()))
	{
		InInterface->OnResetTargetObject(this);
		InInterface->OnClickedObject();
	}
}
