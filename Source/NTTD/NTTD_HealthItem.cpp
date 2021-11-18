// Fill out your copyright notice in the Description page of Project Settings.


#include "NTTD_HealthItem.h"
#include "NTTDCharacter.h"
#include "NTTD_HealthComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

// Sets default values
ANTTD_HealthItem::ANTTD_HealthItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MainColliderComponent = CreateDefaultSubobject<USphereComponent>(TEXT("MainColliderComponent"));
	MainColliderComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MainColliderComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	MainColliderComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	RootComponent = MainColliderComponent;

	HealthBoxMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HealthBoxMeshComponent"));
	HealthBoxMeshComponent->SetupAttachment(RootComponent);
	HealthBoxMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	HealthValue = 20.0f;

}

void ANTTD_HealthItem::NotifyActorBeginOverlap(AActor* OtherActor)
{
	if (IsValid(OtherActor))
	{
		ANTTDCharacter* PossibleCharacter = Cast<ANTTDCharacter>(OtherActor);
		if (IsValid(PossibleCharacter))
		{
			UNTTD_HealthComponent* CharacterHealthComponent = PossibleCharacter->GetHealthComponent();
			if (IsValid(CharacterHealthComponent))
			{
				const bool bIsHealingSuccessfull = CharacterHealthComponent->IsHealthAdded(HealthValue);

				if (bIsHealingSuccessfull)
				{
					PlayHealSound();
					Destroy();
				}

			}
		}
	}
}

// Called when the game starts or when spawned
void ANTTD_HealthItem::BeginPlay()
{
	Super::BeginPlay();
	
}

void ANTTD_HealthItem::PlayHealSound()
{
	if (!IsValid(HealSound))
	{
		return;
	}

	UGameplayStatics::PlaySoundAtLocation(GetWorld(), HealSound, GetActorLocation());
}

// Called every frame
void ANTTD_HealthItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

