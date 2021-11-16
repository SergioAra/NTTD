// Fill out your copyright notice in the Description page of Project Settings.


#include "NTTD_InfectionBar.h"
#include "NTTDCharacter.h"
#include "Kismet/GameplayStatics.h"

void UNTTD_InfectionBar::InitializeWidget()
{
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (IsValid(PlayerPawn))
	{
		ANTTDCharacter* PlayerCharacter = Cast<ANTTDCharacter>(PlayerPawn);
		if (IsValid(PlayerCharacter))
		{
			PlayerCharacter->OnInfectionUpdateDelegate.AddDynamic(this, &UNTTD_InfectionBar::UpdateInfection);
		}

	}
}

void UNTTD_InfectionBar::UpdateInfection(float CurrentInfection, float MaxInfection)
{
	InfectionPercent = CurrentInfection / MaxInfection;
}
