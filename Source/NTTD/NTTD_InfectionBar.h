// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NTTD_InfectionBar.generated.h"

/**
 * 
 */
UCLASS()
class NTTD_API UNTTD_InfectionBar : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(BlueprintReadOnly, Category = "Setup")
	float InfectionPercent;

protected:
	UFUNCTION(BlueprintCallable)
	void InitializeWidget();

	UFUNCTION()
	void UpdateInfection(float CurrentInfection, float MaxInfection);
};
