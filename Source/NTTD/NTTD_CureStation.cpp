// Fill out your copyright notice in the Description page of Project Settings.


#include "NTTD_CureStation.h"

#include "NTTDCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"

// Sets default values
ANTTD_CureStation::ANTTD_CureStation():
	ItemCount(0),
	bIsPlayerOverlapping(false)
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

}

// Called when the game starts or when spawned
void ANTTD_CureStation::BeginPlay()
{
	Super::BeginPlay();

	//Setup overlap for AreaSphere
	AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &ANTTD_CureStation::OnSphereOverlap);
	AreaSphere->OnComponentEndOverlap.AddDynamic(this, &ANTTD_CureStation::OnSphereEndOverlap);
	

	//Set custom depth to disabled
	InitializeCustomDepth();
	
}

void ANTTD_CureStation::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor)
	{
		ANTTDCharacter* NTTDCharacter = Cast<ANTTDCharacter>(OtherActor);
		if(NTTDCharacter)
		{
			bIsPlayerOverlapping = true;
			NTTDCharacter->SetCurrentAmountOfInfection(0.0f);
		}
	}
}

void ANTTD_CureStation::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(OtherActor)
	{
		ANTTDCharacter* NTTDCharacter = Cast<ANTTDCharacter>(OtherActor);
		if(NTTDCharacter)
		{
			NTTDCharacter->IncrementOverlappedItemCount(-1);
			bIsPlayerOverlapping = false;
		}
	}
}


void ANTTD_CureStation::EnableCustomDepth()
{
	ItemMesh->SetRenderCustomDepth(true);
}

void ANTTD_CureStation::DisableCustomDepth()
{
	ItemMesh->SetRenderCustomDepth(false);
}

void ANTTD_CureStation::InitializeCustomDepth()
{
	DisableCustomDepth();
}


// Called every frame
void ANTTD_CureStation::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


