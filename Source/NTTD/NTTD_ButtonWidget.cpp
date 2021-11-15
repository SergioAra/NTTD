// Fill out your copyright notice in the Description page of Project Settings.


#include "NTTD_ButtonWidget.h"

UNTTD_ButtonWidget::UNTTD_ButtonWidget()
{
	CheckFocusRate = 0.1f;
}

void UNTTD_ButtonWidget::Start()
{
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_CheckFocus, this, &UNTTD_ButtonWidget::CheckFocus, CheckFocusRate, true);
}

void UNTTD_ButtonWidget::CheckFocus()
{
	if (IsHovered())
	{
		SetKeyboardFocus();
		SetButtonStyle(ENTTD_ButtonStyleType::ButtonStyleType_Mouse);
	}
	else
	{
		ENTTD_ButtonStyleType StyleTypeSelected = HasKeyboardFocus() ? ENTTD_ButtonStyleType::ButtonStyleType_KeySelected : ENTTD_ButtonStyleType::ButtonStyleType_KeyNotSelected;

	}
}

void UNTTD_ButtonWidget::SetButtonStyle(ENTTD_ButtonStyleType NewStyleType)
{
	BP_SetButtonStyle(NewStyleType)
}
