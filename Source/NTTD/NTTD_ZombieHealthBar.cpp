// Fill out your copyright notice in the Description page of Project Settings.


#include "NTTD_ZombieHealthBar.h"

void UNTTD_ZombieHealthBar::UpdateHealth(float CurrentHealth, float MaxHealth)
{
	Health = CurrentHealth / MaxHealth;
}
