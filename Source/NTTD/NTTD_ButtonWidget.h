// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "NTTD_ButtonWidget.generated.h"

/**
 * 
 */
UENUM(Blueprintable)
enum class ENTTD_ButtonStyleType :uint8
{
	ButtonStyleType_Mouse     UMETA(DisplayName = "Selected by Mouse"),
	ButtonStyleType_KeySelected UMETA(DisplayName = "Selected by Keyboard"),
	ButtonStyleType_KeyNotSelected UMETA(DisplayName = "Not Selected by Keyboard"),
};



UCLASS()
class NTTD_API UNTTD_ButtonWidget : public UButton
{
	GENERATED_BODY()

protected:
	//Variables
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Style")
	float CheckFocusRate;

	FTimerHandle TimerHandle_CheckFocus;

public:
	UNTTD_ButtonWidget();

protected:
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void Start();

	void CheckFocus();

	UFUNCTION(BlueprintCallable, Category = "Style")
	void SetButtonStyle(ENTTD_ButtonStyleType NewStyleType);

	UFUNCTION(BlueprintImplementableEvent, Category = "Style")
	void BP_SetButtonStyle(ENTTD_ButtonStyleType NewStyleType);

	
};
