// Copyright Epic Games, Inc. All Rights Reserved.

#include "MMOARPGCharacter.h"
//#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "../../Component/FlyComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

//////////////////////////////////////////////////////////////////////////
// AMMOARPGCharacter

AMMOARPGCharacter::AMMOARPGCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

void AMMOARPGCharacter::BeginPlay()
{
	Super::BeginPlay();
	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void AMMOARPGCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMMOARPGCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMMOARPGCharacter::Look);

	}

	//// Set up gameplay key bindings
	//check(PlayerInputComponent);
	//PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	//PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	//PlayerInputComponent->BindAction("Fly", IE_Pressed, this, &AMMOARPGCharacter::Fly);
	//PlayerInputComponent->BindAction("Fast", IE_Pressed, this, &AMMOARPGCharacter::Fast);

	//PlayerInputComponent->BindAction("DodgeLeft", IE_Pressed, this, &AMMOARPGCharacter::DodgeLeft);
	//PlayerInputComponent->BindAction("DodgeRight", IE_Pressed, this, &AMMOARPGCharacter::DodgeRight);

	//PlayerInputComponent->BindAction("SwitchFight", IE_Pressed, this, &AMMOARPGCharacter::SwitchFight);

	//PlayerInputComponent->BindAxis("MoveForward", this, &AMMOARPGCharacter::MoveForward);
	//PlayerInputComponent->BindAxis("MoveRight", this, &AMMOARPGCharacter::MoveRight);

	//// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	//// "turn" handles devices that provide an absolute delta, such as a mouse.
	//// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	//PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	//PlayerInputComponent->BindAxis("TurnRate", this, &AMMOARPGCharacter::TurnAtRate);
	//PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	//PlayerInputComponent->BindAxis("LookUpRate", this, &AMMOARPGCharacter::LookUpAtRate);

	//// handle touch devices
	//PlayerInputComponent->BindTouch(IE_Pressed, this, &AMMOARPGCharacter::TouchStarted);
	//PlayerInputComponent->BindTouch(IE_Released, this, &AMMOARPGCharacter::TouchStopped);

	//// VR headset functionality
	//PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AMMOARPGCharacter::OnResetVR);
}
void AMMOARPGCharacter::Move(const FInputActionValue& Value)
{
	//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, FString::Printf(TEXT("Test2")));
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}
void AMMOARPGCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}
void AMMOARPGCharacter::Fast_Implementation()
{
	MulticastFast();
}

void AMMOARPGCharacter::MulticastFast_Implementation()
{
	if (ActionState == ECharacterActionState::FLIGHT_STATE)
	{
		GetFlyComponent()->ResetFastFly();
	}
}

void AMMOARPGCharacter::MulticastDodgeLeft_Implementation()
{
	if (ActionState == ECharacterActionState::FLIGHT_STATE)
	{
		GetFlyComponent()->ResetDodgeFly(EDodgeFly::DODGE_LEFT);
	}
}

void AMMOARPGCharacter::DodgeLeft_Implementation()
{
	MulticastDodgeLeft();
}

void AMMOARPGCharacter::DodgeRight_Implementation()
{
	MulticastDodgeRight();
}

void AMMOARPGCharacter::MulticastDodgeRight_Implementation()
{
	if (ActionState == ECharacterActionState::FLIGHT_STATE)
	{
		GetFlyComponent()->ResetDodgeFly(EDodgeFly::DODGE_RIGHT);
	}
}

void AMMOARPGCharacter::MulticastFly_Implementation()
{
	ResetActionState(ECharacterActionState::FLIGHT_STATE);

	GetFlyComponent()->ResetFly();
}

void AMMOARPGCharacter::Fly_Implementation()
{
	MulticastFly();
}

void AMMOARPGCharacter::SwitchFight()
{
	//客户端
	ResetActionState(ECharacterActionState::FIGHT_STATE);

	//客户端先播放
	FightChanged();

	//通知服务器
	SwitchActionStateOnServer(ActionState);

	LastActionState = ActionState;
}

void AMMOARPGCharacter::OnResetVR()
{
	// If MMOARPG is added to a project via 'Add Feature' in the Unreal Editor the dependency on HeadMountedDisplay in MMOARPG.Build.cs is not automatically propagated
	// and a linker error will result.
	// You will need to either:
	//		Add "HeadMountedDisplay" to [YourProject].Build.cs PublicDependencyModuleNames in order to build successfully (appropriate if supporting VR).
	// or:
	//		Comment or delete the call to ResetOrientationAndPosition below (appropriate if not supporting VR)
//	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AMMOARPGCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector InLocation)
{
	Jump();
}

void AMMOARPGCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector InLocation)
{
	StopJumping();
}

void AMMOARPGCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMMOARPGCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AMMOARPGCharacter::MoveForward(float Value)
{
	if (Controller != nullptr)
	{
		if (ActionState == ECharacterActionState::FLIGHT_STATE)
		{
			GetFlyComponent()->FlyForwardAxis(Value);
		}
		else
		{
			// find out which way is forward
			const FRotator Rotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);

			// get forward vector
			const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
			AddMovementInput(Direction, Value);
		}
	}
}

void AMMOARPGCharacter::MoveRight(float Value)
{
	if ( (Controller != nullptr) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void AMMOARPGCharacter::FightChanged()
{
	if (FCharacterAnimTable* InAnimTable = GetAnimTable())
	{
		if (InAnimTable->SwitchFightMontage)
		{
			PlayAnimMontage(InAnimTable->SwitchFightMontage, 1.f,
				ActionState == ECharacterActionState::FIGHT_STATE ? TEXT("0") : TEXT("1"));
		}
	}
}

void AMMOARPGCharacter::OnRep_ActionStateChanged()
{
	if (GetLocalRole() != ROLE_Authority)
	{
		if (ActionState == ECharacterActionState::FIGHT_STATE || 
			LastActionState == ECharacterActionState::FIGHT_STATE)
		{
			FightChanged();
		}

		LastActionState = ActionState;
	}
}