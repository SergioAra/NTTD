// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "NTTD_GameMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameStateChangeSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMinutesChangeSignature, int, CurrentMinutes);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSecondsChangeSignature, int, CurrentSeconds);

/**
 * 
 */
class ANTTDCharacter;
class UAudioComponent;
class USoundCue;
UCLASS()
class NTTD_API ANTTD_GameMode : public AGameModeBase
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UAudioComponent* StateSoundComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio")
	USoundCue* VictorySound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio")
	USoundCue* GameOverSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio")
	USoundCue* GameOverVoiceSound;

protected:
	//Variables

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameFlow")
	float TimeToGoBackToMenuAfterVictory;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameFlow")
	float TimeToGoBackToMenuAfterGameOver;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Timer", meta = (ClampMin = 0.0f, ClampMax = 60.0f))
	int Minutes;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Timer", meta = (ClampMin = 0.0f, ClampMax = 60.0f))
	int Seconds;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameFlow")
	FName MainMenuMapName;

	FTimerHandle TimerHandle_UpdateTime;

	FTimerHandle TimerHandle_BackToMainMenu;

	FTimerHandle TimerHandle_PlayVoice;

	FTimerDelegate TimerDelegate_PlayVoice;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void PlayStateSound(USoundCue* SoundCue);

	void UpdateStartingTime();

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

	UFUNCTION(BlueprintCallable)
	void DecreaseTime();

	//UFUNCTION(BlueprintImplementableEvent)
	//void BP_Delay();

public:
	UPROPERTY(BlueprintAssignable)
	FOnGameStateChangeSignature OnVictoryDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnGameStateChangeSignature OnGameOverDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnMinutesChangeSignature OnMinutesChangeDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnSecondsChangeSignature OnSecondsChangeDelegate;

};
