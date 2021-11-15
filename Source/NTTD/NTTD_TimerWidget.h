// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NTTD_TimerWidget.generated.h"

/**
 * 
 */

class ANTTD_GameMode;

UCLASS()
class NTTD_API UNTTD_TimerWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings")
	int CurrentMinutes;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings")
	int CurrentSeconds;

	ANTTD_GameMode* MyGameModeReference;

	UFUNCTION(BlueprintCallable)
	void InitializeWidget();

	UFUNCTION()
	void UpdateMinutes(int NewMinutes);

	UFUNCTION()
	void UpdateSeconds(int NewSeconds);
	
};
