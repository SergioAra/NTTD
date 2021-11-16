// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NTTD_MainMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class NTTD_API UNTTD_MainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	//variables
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Main Menu")
	FName  GamePlayLevelName;


protected:
	UFUNCTION(BlueprintCallable, Category = "Main Menu")
	void NewGame();

	UFUNCTION(BlueprintCallable, Category = "Main Menu")
	void Controls();

	UFUNCTION(BlueprintCallable, Category = "Main Menu")
	void Credits();

	UFUNCTION(BlueprintCallable, Category = "Main Menu")
	void QuitGame();

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "Main Menu")
	void BP_NewGame();

	UFUNCTION(BlueprintImplementableEvent, Category = "Main Menu")
	void BP_Controls();

	UFUNCTION(BlueprintImplementableEvent, Category = "Main Menu")
	void BP_Credits();

	UFUNCTION(BlueprintImplementableEvent, Category = "Main Menu")
	void BP_QuitGame();

	
};
