// Copyright Epic Games, Inc. All Rights Reserved.

#include "NTTDGameMode.h"
#include "NTTDPlayerController.h"
#include "NTTDCharacter.h"
#include "UObject/ConstructorHelpers.h"

ANTTDGameMode::ANTTDGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = ANTTDPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDownCPP/Blueprints/TopDownCharacter"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}