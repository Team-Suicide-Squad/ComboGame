// Fill out your copyright notice in the Description page of Project Settings.


#include "AttributeSets/CharacterAttributeSet.h"

void UCharacterAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, MAXVALUE);
	}

	if (Attribute == GetMaxHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, MAXVALUE);
	}

	if (Attribute == GetManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, MAXVALUE);
	}

	if (Attribute == GetMaxManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, MAXVALUE);
	}

	if (Attribute == GetSpeedAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, MAXVALUE);
	}

	if (Attribute == GetMaxSpeedAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, MAXVALUE);
	}
}

void UCharacterAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	if(Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, MAXVALUE);
	}

	if (Attribute == GetMaxHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, MAXVALUE);
	}

	if(Attribute == GetManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, MAXVALUE);
	}

	if (Attribute == GetMaxManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, MAXVALUE);
	}

	if (Attribute == GetSpeedAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, MAXVALUE);
	}

	if (Attribute == GetMaxSpeedAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, MAXVALUE);
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
