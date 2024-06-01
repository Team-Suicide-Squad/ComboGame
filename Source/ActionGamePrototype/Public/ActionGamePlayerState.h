// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "ActionGamePlayerState.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONGAMEPROTOTYPE_API AActionGamePlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GAS, meta = (AllowPrivateAccess = "true"))
	UAbilitySystemComponent* AbilitySystemComponent;

public: 
	AActionGamePlayerState();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
};
