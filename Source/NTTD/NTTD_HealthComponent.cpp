// Fill out your copyright notice in the Description page of Project Settings.


#include "NTTD_HealthComponent.h"

// Sets default values for this component's properties
UNTTD_HealthComponent::UNTTD_HealthComponent()
{
	MaxHealth = 100.0f;
}


bool UNTTD_HealthComponent::IsHealthAdded(float HealthToAdd)
{
	if (bIsDead)
	{
		return false;
	}

	if (CurrentHealth == MaxHealth)
	{
		return false;
	}

	CurrentHealth = FMath::Clamp((CurrentHealth + HealthToAdd), 0.0f, MaxHealth);
	OnHealthUpdateDelegate.Broadcast(CurrentHealth, MaxHealth);

	return true;
}

// Called when the game starts
void UNTTD_HealthComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;
	MyOwner = GetOwner();

	if (IsValid(MyOwner))
	{
		//Bind Health Component delegate to owner
		MyOwner->OnTakeAnyDamage.AddDynamic(this, &UNTTD_HealthComponent::TakingDamage);
	}

	GetWorld()->GetTimerManager().SetTimer(TimerHanlde_UpdateInitialHealth, this, &UNTTD_HealthComponent::UpdateInitialHealth, 0.1f, false);
	
}

void UNTTD_HealthComponent::TakingDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage < 0.0f)
	{
		return;
	}

	CurrentHealth = FMath::Clamp((CurrentHealth - Damage), 0.0f, MaxHealth);

	if (CurrentHealth == 0.0f)
	{
		bIsDead = true;
		OnDeadDelegate.Broadcast(DamageCauser);
	}

	OnHealthChangeDelegate.Broadcast(this, DamagedActor, Damage, DamageType, InstigatedBy, DamageCauser);
	OnHealthUpdateDelegate.Broadcast(CurrentHealth, MaxHealth);
}

void UNTTD_HealthComponent::UpdateInitialHealth()
{
	OnHealthUpdateDelegate.Broadcast(CurrentHealth, MaxHealth);
}



