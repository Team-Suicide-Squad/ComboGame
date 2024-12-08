// Copyright Epic Games, Inc. All Rights Reserved.

#include "ActionGamePrototypeGameMode.h"
#include "ActionGamePrototypeCharacter.h"
#include "UObject/ConstructorHelpers.h"

AActionGamePrototypeGameMode::AActionGamePrototypeGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Base/BP_ActionGameCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
