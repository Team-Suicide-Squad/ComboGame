// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_Dash.h"
#include "ActionGamePrototype/ActionGamePrototypeCharacter.h"

UGA_Dash::UGA_Dash(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_Dash::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
		{
			return;
		}

		AActionGamePrototypeCharacter* Character = CastChecked<AActionGamePrototypeCharacter>(ActorInfo->AvatarActor.Get());
		Character->Dash(DashDistance);

		if (UGameInstance* GameInstance = GetGameInstance())
		{
			GameInstance->GetTimerManager().SetTimer(DashTimer, this, &UGA_Dash::ResetDash, TimeBetweenDashes, false);
		}
	}
}

void UGA_Dash::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (ActorInfo != NULL && ActorInfo->AvatarActor != NULL)
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
	}
}

bool UGA_Dash::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	return true;
}

/**
 *	Canceling an non instanced ability is tricky. Right now this works for Jump since there is nothing that can go wrong by calling
 *	StopJumping() if you aren't already jumping. If we had a montage playing non instanced ability, it would need to make sure the
 *	Montage that *it* played was still playing, and if so, to cancel it. If this is something we need to support, we may need some
 *	light weight data structure to represent 'non intanced abilities in action' with a way to cancel/end them.
 */
void UGA_Dash::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	if (ScopeLockCount > 0)
	{
		WaitingToExecute.Add(FPostLockDelegate::CreateUObject(this, &UGA_Dash::CancelAbility, Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility));
		return;
	}

	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);

	AActionGamePrototypeCharacter* Character = CastChecked<AActionGamePrototypeCharacter>(ActorInfo->AvatarActor.Get());
	Character->StopDash();
}

void UGA_Dash::ResetDash()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

UGameInstance* UGA_Dash::GetGameInstance()
{
	UWorld* World = GetWorld();
	if (!World && GetCurrentActorInfo())
	{
		World = GetCurrentActorInfo()->AvatarActor.IsValid() ? GetCurrentActorInfo()->AvatarActor->GetWorld() : nullptr;
	}

	if (World)
	{
		return World->GetGameInstance();
	}

	return nullptr;
}
