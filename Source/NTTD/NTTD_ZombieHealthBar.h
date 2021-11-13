// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NTTD_ZombieHealthBar.generated.h"

/**
 * 
 */
UCLASS()
class NTTD_API UNTTD_ZombieHealthBar : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Settings")
	float Health;

	UFUNCTION()
	void UpdateHealth(float CurrentHealth, float MaxHealth);
};
