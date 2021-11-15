// Fill out your copyright notice in the Description page of Project Settings.


#include "NTTD_PlayerHealthBar.h"
#include "NTTDCharacter.h"
#include "NTTD_HealthComponent.h"
#include "Kismet/GameplayStatics.h"

void UNTTD_PlayerHealthBar::InitializeWidget()
{
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (IsValid(PlayerPawn))
	{
		ANTTDCharacter* PlayerCharacter = Cast<ANTTDCharacter>(PlayerPawn);
		if (IsValid(PlayerCharacter))
		{
			UNTTD_HealthComponent* PlayerHealthComponent = Cast<UNTTD_HealthComponent>(PlayerCharacter->GetHealthComponent());
			if (IsValid(PlayerHealthComponent))
			{
				PlayerHealthComponent->OnHealthUpdateDelegate.AddDynamic(this, &UNTTD_PlayerHealthBar::UpdateHealth);
			}

		}

	}
}

void UNTTD_PlayerHealthBar::UpdateHealth(float CurrentHealth, float MaxHealth)
{
	HealthPercent = CurrentHealth / MaxHealth;

	HealthColor = FMath::Lerp(EmptyHealthColor, FullHealthColor, HealthPercent);
}
