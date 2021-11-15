// Fill out your copyright notice in the Description page of Project Settings.


#include "NTTD_TimerWidget.h"
#include "NTTD_GameMode.h"

void UNTTD_TimerWidget::InitializeWidget()
{
	MyGameModeReference = Cast<ANTTD_GameMode>(GetWorld()->GetAuthGameMode());
	if (IsValid(MyGameModeReference))
	{
		MyGameModeReference->OnMinutesChangeDelegate.AddDynamic(this, &UNTTD_TimerWidget::UpdateMinutes);
		MyGameModeReference->OnSecondsChangeDelegate.AddDynamic(this, &UNTTD_TimerWidget::UpdateSeconds);
	}
}

void UNTTD_TimerWidget::UpdateMinutes(int NewMinutes)
{
	CurrentMinutes = NewMinutes;
}

void UNTTD_TimerWidget::UpdateSeconds(int NewSeconds)
{
	CurrentSeconds = NewSeconds;
}
