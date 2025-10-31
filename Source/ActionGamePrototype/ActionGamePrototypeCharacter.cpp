// Copyright Epic Games, Inc. All Rights Reserved.

#include "ActionGamePrototypeCharacter.h"

#include "AbilitySystemComponent.h"
#include "AttributeSets/CharacterAttributeSet.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/LocalPlayer.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GA_Jump.h"
#include "GA_Dash.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputActionValue.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AActionGamePrototypeCharacter

AActionGamePrototypeCharacter::AActionGamePrototypeCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	JumpMaxCount = 2;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Create Ability System Component and set replication
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);

	// Listeners bindings. I put those here because the Init GE is executed before the BeginPlay
	OnHealthAttributeChangeDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCharacterAttributeSet::GetHealthAttribute()).AddUObject(this, &AActionGamePrototypeCharacter::OnHealthChanged);
	OnManaAttributeChangeDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCharacterAttributeSet::GetManaAttribute()).AddUObject(this, &AActionGamePrototypeCharacter::OnManaChanged);
	OnSpeedAttributeChangeDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCharacterAttributeSet::GetSpeedAttribute()).AddUObject(this, &AActionGamePrototypeCharacter::OnSpeedChanged);

	// Create the Character attribute set
	CharacterAttributeSet = CreateDefaultSubobject<UCharacterAttributeSet>(TEXT("CharacterAttributeSet"));

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void AActionGamePrototypeCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(JumpGameplayAbility, 1, 0));
	AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(DashGameplayAbility, 1, 0));
}

void AActionGamePrototypeCharacter::Jump()
{
	Super::Jump();
}

void AActionGamePrototypeCharacter::Dash(float DashDistance)
{
	FVector DashDirection = GetLastMovementInputVector();
	if (DashDirection.IsNearlyZero())
	{
		DashDirection = GetActorForwardVector();
	}

	GetCharacterMovement()->MovementMode = EMovementMode::MOVE_Custom;
	LaunchCharacter(DashDirection.GetSafeNormal() * DashDistance, true, true);
}

void AActionGamePrototypeCharacter::StopDash()
{
	GetCharacterMovement()->StopActiveMovement();
}

bool AActionGamePrototypeCharacter::CanJump() const
{
	return Super::CanJump();
}

void AActionGamePrototypeCharacter::StopJumping()
{
	Super::StopJumping();
}

bool AActionGamePrototypeCharacter::CanJumpInternal_Implementation() const
{
	return Super::CanJumpInternal_Implementation();
}

//////////////////////////////////////////////////////////////////////////
// Input

void AActionGamePrototypeCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AActionGamePrototypeCharacter::JumpInput);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AActionGamePrototypeCharacter::StopJumping);

		// Dashing
		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Started, this, &AActionGamePrototypeCharacter::DashInput);
		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Completed, this, &AActionGamePrototypeCharacter::StopDash);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AActionGamePrototypeCharacter::MoveInput);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AActionGamePrototypeCharacter::LookInput);

		// Attacking
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &AActionGamePrototypeCharacter::AttackInput);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AActionGamePrototypeCharacter::MoveInput(const FInputActionValue& Value)
{
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

void AActionGamePrototypeCharacter::JumpInput(const FInputActionValue& Value)
{
	if(AbilitySystemComponent != nullptr)
	{
		AbilitySystemComponent->TryActivateAbilityByClass(JumpGameplayAbility);
	}
}

void AActionGamePrototypeCharacter::DashInput(const FInputActionValue& Value)
{
	if (AbilitySystemComponent != nullptr)
	{
		AbilitySystemComponent->TryActivateAbilityByClass(DashGameplayAbility);
	}
}

void AActionGamePrototypeCharacter::LookInput(const FInputActionValue& Value)
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

void AActionGamePrototypeCharacter::AttackInput(const FInputActionValue& Value)
{
	// To implement
}


//////////////////////////////////////////////////////////////////////////
// Character state update

void AActionGamePrototypeCharacter::OnHealthChanged(const FOnAttributeChangeData& Data)
{
	const float NewHealth = Data.NewValue;

	if(NewHealth <= 0.0f)
	{
		Die();
	}
	else if(NewHealth < Data.OldValue)
	{
		// TODO: Change this to be relative to some damages only and use the normal with the overlap
		FVector MovementDirection = GetLastMovementInputVector();
		if (MovementDirection.IsNearlyZero())
		{
			MovementDirection = GetActorForwardVector();
		}

		GetCharacterMovement()->MovementMode = EMovementMode::MOVE_Custom;
		LaunchCharacter(MovementDirection.GetSafeNormal() * (-DamageForceReaction), true, true);
	}
}

void AActionGamePrototypeCharacter::OnManaChanged(const FOnAttributeChangeData& Data)
{
	const float NewMana = Data.NewValue;
}

void AActionGamePrototypeCharacter::OnSpeedChanged(const FOnAttributeChangeData& Data)
{
	GetCharacterMovement()->MaxWalkSpeed = Data.NewValue;
}

void AActionGamePrototypeCharacter::Die()
{
	GetCharacterMovement()->Deactivate();
	OnDeath.Broadcast();
}
