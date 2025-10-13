// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameplayEffectTypes.h"
#include "Logging/LogMacros.h"
#include "ActionGamePrototypeCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UAbilitySystemComponent;
class UCharacterAttributeSet;
class UGA_Jump;
class UGA_Dash;
class UInputMappingContext;
class UInputAction;
class UUserWidget;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class AActionGamePrototypeCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	// GAS variables --------------------------
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = GAS, meta = (AllowPrivateAccess = "true"))
	UAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = GAS, meta = (AllowPrivateAccess = "true"))
	UCharacterAttributeSet* CharacterAttributeSet;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GAS, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UGA_Jump> JumpGameplayAbility;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GAS, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UGA_Dash> DashGameplayAbility;
	// ----------------------------------------
	
	// GAS variables --------------------------
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = UI, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> DeathScreenWidget;
	// ----------------------------------------

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	/** Dash Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* DashAction;

	/** Attack Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* AttackAction;

	FDelegateHandle OnHealthAttributeChangeDelegateHandle;
	FDelegateHandle OnManaAttributeChangeDelegateHandle;
	FDelegateHandle OnSpeedAttributeChangeDelegateHandle;

public:
	AActionGamePrototypeCharacter();

	// Dash logic
	void Dash(float DashDistance);
	void StopDash();

	// Jump logic
	void Jump() override;

	// I don't override the function because I don't have the engine code downloaded
	bool CanJump() const;

	void StopJumping() override;

	/* Attributes update */
	void OnHealthChanged(const FOnAttributeChangeData& Data);
	void OnManaChanged(const FOnAttributeChangeData& Data);
	void OnSpeedChanged(const FOnAttributeChangeData& Data);

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

protected:
	// To add mapping context
	void BeginPlay() override;

	// APawn interface
	void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Called for movement input */
	void MoveInput(const FInputActionValue& Value);

	/** Called for looking input */
	void LookInput(const FInputActionValue& Value);

	/** Called for jump input */
	void JumpInput(const FInputActionValue& Value);

	/** Called for dash input */
	void DashInput(const FInputActionValue& Value);

	/** Called for attack input */
	void AttackInput(const FInputActionValue& Value);

	bool CanJumpInternal_Implementation() const override;
			
private:
	UUserWidget* DeathWidgetInstance;

	void Die();
};

