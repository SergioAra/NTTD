// Fill out your copyright notice in the Description page of Project Settings.


#include "NTTD_MainMenuWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void UNTTD_MainMenuWidget::NewGame()
{
	BP_NewGame();
	UGameplayStatics::OpenLevel(GetWorld(), GamePlayLevelName);
}

void UNTTD_MainMenuWidget::Controls()
{
	BP_Controls();
}

void UNTTD_MainMenuWidget::Credits()
{
	BP_Credits();
}

void UNTTD_MainMenuWidget::QuitGame()
{
	BP_QuitGame();
	APlayerController* CurrentPlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	UKismetSystemLibrary::QuitGame(GetWorld(), CurrentPlayerController, EQuitPreference::Quit, false);
}
