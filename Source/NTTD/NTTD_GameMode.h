// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "NTTD_GameMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameStateChangeSignature);

/**
 * 
 */
class ANTTDCharacter;
UCLASS()
class NTTD_API ANTTD_GameMode : public AGameModeBase
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UAudioComponent* VictorySoundComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UAudioComponent* GameOverSoundComponent;

protected:
	//Variables

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameFlow")
	float TimeToGoBackToMenuAfterVictory;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameFlow")
	float TimeToGoBackToMenuAfterGameOver;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameFlow")
	FName MainMenuMapName;

	FTimerHandle TimerHandle_BackToMainMenu;

public:
	ANTTD_GameMode();

	UFUNCTION()
	void Victory(ANTTDCharacter* Character);

	UFUNCTION()
	void GameOver(ANTTDCharacter* Character);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void BP_Victory(ANTTDCharacter* Character);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void BP_GameOver(ANTTDCharacter* Character);

	void BackToMainMenu();

public:
	UPROPERTY(BlueprintAssignable)
	FOnGameStateChangeSignature OnVictoryDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnGameStateChangeSignature OnGameOverDelegate;
};
