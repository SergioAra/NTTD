// Fill out your copyright notice in the Description page of Project Settings.


#include "NTTD_GameMode.h"
#include "Components/CapsuleComponent.h"
#include "Components/AudioComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NTTDPlayerController.h"
#include "NTTDCharacter.h"
#include "UObject/ConstructorHelpers.h"

ANTTD_GameMode::ANTTD_GameMode()
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

void ANTTD_GameMode::Victory(ANTTDCharacter* Character)
{
	BP_Victory(Character);
}

void ANTTD_GameMode::GameOver(ANTTDCharacter* Character)
{
	BP_GameOver(Character);
}

void ANTTD_GameMode::BackToMainMenu()
{
	UGameplayStatics::OpenLevel(GetWorld(), MainMenuMapName);
}
