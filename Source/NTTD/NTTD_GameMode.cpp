// Fill out your copyright notice in the Description page of Project Settings.


#include "NTTD_GameMode.h"
#include "Components/CapsuleComponent.h"
#include "Components/AudioComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NTTDPlayerController.h"
#include "NTTDCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Sound/SoundCue.h"




ANTTD_GameMode::ANTTD_GameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = ANTTDPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDownCPP/Blueprints/TopDownCharacter"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	StateSoundComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("StateSoundComponent"));
	StateSoundComponent->SetupAttachment(RootComponent);

	Minutes = 1.0f;
	Seconds = 0.0f;

}

void ANTTD_GameMode::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetTimerManager().SetTimer(TimerHandle_UpdateTime, this, &ANTTD_GameMode::UpdateStartingTime, 0.1f, false);

}

void ANTTD_GameMode::Victory(ANTTDCharacter* Character)
{
	BP_Victory(Character);
}

void ANTTD_GameMode::GameOver(ANTTDCharacter* Character)
{
	BP_GameOver(Character);
	Character->GetMovementComponent()->StopMovementImmediately();
	Character->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (IsValid(GameOverSound))
	{
		PlayStateSound(GameOverSound);
		if (IsValid(GameOverVoiceSound))
		{
			TimerDelegate_PlayVoice.BindUFunction(this, FName("PlayStateSound"), GameOverVoiceSound);
			GetWorld()->GetTimerManager().SetTimer(TimerHandle_PlayVoice, TimerDelegate_PlayVoice, (GameOverSound->GetDuration()), false);
		}
	}
	
	//GetWorld()->GetTimerManager().SetTimer(TimerHandle_BackToMainMenu, this, &ANTTD_GameMode::BackToMainMenu, TimeToGoBackToMenuAfterGameOver, false);
}

void ANTTD_GameMode::BackToMainMenu()
{
	UGameplayStatics::OpenLevel(GetWorld(), MainMenuMapName);
}

void ANTTD_GameMode::DecreaseTime()
{
	if (Seconds == 0)
	{
		if (Minutes == 0)
		{
			OnSecondsChangeDelegate.Broadcast(Seconds);
			APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
			if (IsValid(PlayerPawn))
			{
				ANTTDCharacter* PlayerCharacter = Cast<ANTTDCharacter>(PlayerPawn);
				if (IsValid(PlayerCharacter))
				{
					Victory(PlayerCharacter);
				}

			}
		}
		else
		{
			Seconds = 59;
			Minutes -= 1;
			OnSecondsChangeDelegate.Broadcast(Seconds);
			OnMinutesChangeDelegate.Broadcast(Minutes);
		}
	}
	else
	{
		Seconds -= 1;
		OnSecondsChangeDelegate.Broadcast(Seconds);
		OnMinutesChangeDelegate.Broadcast(Minutes);
	}
}

void ANTTD_GameMode::PlayStateSound(USoundCue* SoundCue)
{
	if (!IsValid(SoundCue))
	{
		return;
	}

	StateSoundComponent->SetSound(SoundCue);
	StateSoundComponent->Play();

}

void ANTTD_GameMode::UpdateStartingTime()
{
	OnSecondsChangeDelegate.Broadcast(Seconds);
	OnMinutesChangeDelegate.Broadcast(Minutes);
}

