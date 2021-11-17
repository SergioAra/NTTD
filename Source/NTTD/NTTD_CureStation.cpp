// Fill out your copyright notice in the Description page of Project Settings.


#include "NTTD_CureStation.h"

#include "NTTDCharacter.h"
#include "Components/AudioComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"

// Sets default values
ANTTD_CureStation::ANTTD_CureStation():
	MaxCharge(50.0f),
	CurrentRate(5.f),
	ChargeRate(3.f),
	DischargeRate(-5.f),
	CurrentAmountOfCharge(0.f),
	bIsPlayerOverlapping(false),
	bShouldCure(false),
	bChargeSoundPlayed(true),
	bDischargeSoundPlayed(true)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	SetRootComponent(ItemMesh);

	SecondItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SecondItemMesh"));
	SecondItemMesh->SetupAttachment(GetRootComponent());
	
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetupAttachment(GetRootComponent());
	//block collision to all channels
	CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	//set collision to block visibility channel
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(GetRootComponent());
	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	AudioComponent->SetupAttachment(GetRootComponent());

	CureLoopComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("HealingAudioComponent"));
	CureLoopComponent->SetupAttachment(GetRootComponent());
	
}

// Called when the game starts or when spawned
void ANTTD_CureStation::BeginPlay()
{
	Super::BeginPlay();

	//Setup overlap for AreaSphere
	AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &ANTTD_CureStation::OnSphereOverlap);
	AreaSphere->OnComponentEndOverlap.AddDynamic(this, &ANTTD_CureStation::OnSphereEndOverlap);

	GetWorld()->GetTimerManager().SetTimer(TimerHandle_UpdateCharge, this, &ANTTD_CureStation::UpdateInitialCharge, 0.2f, false);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_ChargeGrowth, this, &ANTTD_CureStation::Recharge, 1.0f, true);
	
	InitializeCustomDepth();

	CureLoopComponent->SetSound(CureLoopSound);
	
}

void ANTTD_CureStation::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
	if(Cast<ANTTDCharacter>(OtherActor))
	{
		CuredCharacter = Cast<ANTTDCharacter>(OtherActor);
		if(CuredCharacter)
		{
			bIsPlayerOverlapping = true;
			if(bShouldCure)
			{
				Cure();
			}
		}
	}
}

void ANTTD_CureStation::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(Cast<ANTTDCharacter>(OtherActor))
	{
		CuredCharacter = Cast<ANTTDCharacter>(OtherActor);
		if(CuredCharacter)
		{
			bIsPlayerOverlapping = false;
			if(bShouldCure)
			{
				StopCure();
			}
			
		}
	}
}


void ANTTD_CureStation::EnableCustomDepth()
{
	ItemMesh->SetRenderCustomDepth(true);
	SecondItemMesh->SetRenderCustomDepth(true);
}

void ANTTD_CureStation::DisableCustomDepth()
{
	ItemMesh->SetRenderCustomDepth(false);
	SecondItemMesh->SetRenderCustomDepth(false);
}

void ANTTD_CureStation::InitializeCustomDepth()
{
	DisableCustomDepth();
}



// Called every frame
void ANTTD_CureStation::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CureSwitch();
	HighlightSetOnCharge();
	Highlight();
	SpawnBeam();
	CureSoundSwitch();
	ChargeSoundSwitch();

}

void ANTTD_CureStation::Recharge()
{
	if (CurrentAmountOfCharge >= 0)
	{
		if(CurrentAmountOfCharge + CurrentRate >= MaxCharge)
		{
			CurrentAmountOfCharge = MaxCharge;
			if(bIsPlayerOverlapping)
			{
				Cure();
			}
		}else if(CurrentAmountOfCharge + CurrentRate <= 0)
		{
			CurrentAmountOfCharge = 0;
			if(bIsPlayerOverlapping)
			{
				StopCure();
			}
		}else
		{
			CurrentAmountOfCharge += CurrentRate;
			bChargeSoundPlayed = false;
			bDischargeSoundPlayed = false;
		}
		//update if we get a widget
	}
}

void ANTTD_CureStation::UpdateInitialCharge()
{
	//send data if widget
}

void ANTTD_CureStation::Cure()
{
	if(CuredCharacter)
	{
		CurrentRate = DischargeRate;
		CuredCharacter->SetInfectionRate(-abs(CuredCharacter->GetInfectionRate()));

		
	}
}

void ANTTD_CureStation::StopCure()
{
	if(CuredCharacter)
	{
		CurrentRate = ChargeRate;
		CuredCharacter->SetInfectionRate(abs(CuredCharacter->GetInfectionRate()));
	}
	
}

void ANTTD_CureStation::Highlight()
{
	if(bShouldCure)
	{
		EnableCustomDepth();
	}else
	{
		DisableCustomDepth();
	}
}

void ANTTD_CureStation::HighlightSetOnCharge()
{
	const float Percentage = CurrentAmountOfCharge/MaxCharge;
	if(Percentage > 0.3f)
	{
		if(Percentage == 1)
		{
			FullEnergyHighlight();
		}else
		{
			if(bIsPlayerOverlapping)
			{
				FullEnergyHighlight();
			}else
			{
				ChargingHighlight();
			}
		}
	}else
	{
		LowEnergyHighlight();
	}
	
}

void ANTTD_CureStation::FullEnergyHighlight()
{
	ItemMesh->SetCustomDepthStencilValue(252);
	SecondItemMesh->SetCustomDepthStencilValue(252);
}

void ANTTD_CureStation::LowEnergyHighlight()
{
	ItemMesh->SetCustomDepthStencilValue(250);
	SecondItemMesh->SetCustomDepthStencilValue(250);
}

void ANTTD_CureStation::ChargingHighlight()
{
	ItemMesh->SetCustomDepthStencilValue(254);
	SecondItemMesh->SetCustomDepthStencilValue(254);
}

void ANTTD_CureStation::CureSwitch()
{
	if(CurrentAmountOfCharge >= MaxCharge)
	{
		bShouldCure = true;
	}
	if(CurrentAmountOfCharge <= 0)
	{
		bShouldCure = false;
	}
}

void ANTTD_CureStation::SpawnBeam()
{
	if(!CuredCharacter) return;
	if(!bShouldCure) return;
	if(!bIsPlayerOverlapping) return;
	if(BeamParticles)
	{
		UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BeamParticles, SecondItemMesh->GetSocketTransform("CureBeam"));
		if (Beam)
		{
			Beam->SetVectorParameter(FName("Target"), CuredCharacter->GetTransform().GetLocation());
		}
	}
}

void ANTTD_CureStation::CureSoundSwitch()
{
	if(!CureLoopSound) return;
	if(!CureLoopComponent) return;
	if(!CuredCharacter) return;
	
	if(bShouldCure)
	{
		if(bIsPlayerOverlapping)
		{
			if(!CureLoopComponent->IsPlaying())
				CureLoopComponent->Play();
		}else
		{
			if(CureLoopComponent->IsPlaying())
				CureLoopComponent->Stop();
		}
	}else
	{
		if(CureLoopComponent->IsPlaying())
			CureLoopComponent->Stop();
	}
}

void ANTTD_CureStation::ChargeSoundSwitch()
{
	if(!ChargedSound) return;
	if(!DischargedSound) return;
	if(!AudioComponent) return;
	
	if(CurrentAmountOfCharge >= MaxCharge)
	{
		if(!bChargeSoundPlayed)
		{
			AudioComponent->SetSound(ChargedSound);
			AudioComponent->Play();
			bChargeSoundPlayed = true;
		}
	}
	if(CurrentAmountOfCharge <= 0)
	{
		if(!bDischargeSoundPlayed)
		{
			AudioComponent->SetSound(DischargedSound);
			AudioComponent->Play();
			bDischargeSoundPlayed = true;
		}
			
	}
	
}


