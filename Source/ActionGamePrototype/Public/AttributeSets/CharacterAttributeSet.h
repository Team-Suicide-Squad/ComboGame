// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "CharacterAttributeSet.generated.h"

// Uses macros from AttributeSet.h
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * 
 */
UCLASS()
class ACTIONGAMEPROTOTYPE_API UCharacterAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
#pragma region Health
	UPROPERTY(BlueprintReadOnly, Category = "Health")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UCharacterAttributeSet, Health)

	UPROPERTY(BlueprintReadOnly, Category = "Health")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UCharacterAttributeSet, MaxHealth)
#pragma endregion

#pragma region Mana
	UPROPERTY(BlueprintReadOnly, Category = "Mana")
	FGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS(UCharacterAttributeSet, Mana)

	UPROPERTY(BlueprintReadOnly, Category = "Mana")
	FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS(UCharacterAttributeSet, MaxMana)
#pragma endregion

#pragma region Movement
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	FGameplayAttributeData Speed;
	ATTRIBUTE_ACCESSORS(UCharacterAttributeSet, Speed)

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	FGameplayAttributeData MaxSpeed;
	ATTRIBUTE_ACCESSORS(UCharacterAttributeSet, MaxSpeed)
#pragma endregion

	void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	UFUNCTION(BlueprintCallable)
	float GetHealthPercentage();

	UFUNCTION(BlueprintCallable)
	float GetManaPercentage();

private:
	static constexpr float MAXVALUE = 100;
	static constexpr float MAXSPEEDVALUE = 9999;
};
