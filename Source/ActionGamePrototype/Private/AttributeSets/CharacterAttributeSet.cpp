// Fill out your copyright notice in the Description page of Project Settings.


#include "AttributeSets/CharacterAttributeSet.h"

UCharacterAttributeSet::UCharacterAttributeSet()
{
	Health.SetBaseValue(100.0f);
	Health.SetCurrentValue(100.0f);
	MaxHealth.SetBaseValue(100.0f);
	MaxHealth.SetCurrentValue(100.0f);

	Mana.SetBaseValue(100.0f);
	Mana.SetCurrentValue(100.0f);
	MaxMana.SetBaseValue(100.0f);
	MaxMana.SetCurrentValue(100.0f);
}

void UCharacterAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	if(Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, 100.0f);
	}

	if (Attribute == GetMaxHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, 100.0f);
	}

	if(Attribute == GetManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, 100.0f);
	}

	if (Attribute == GetMaxManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, 100.0f);
	}
}

float UCharacterAttributeSet::GetHealthPercentage()
{
	return Health.GetCurrentValue()/MaxHealth.GetCurrentValue();
}

float UCharacterAttributeSet::GetManaPercentage()
{
	return Mana.GetCurrentValue()/MaxMana.GetCurrentValue();
}
