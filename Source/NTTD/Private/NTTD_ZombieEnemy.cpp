// Fill out your copyright notice in the Description page of Project Settings.


#include "NTTD_ZombieEnemy.h"
#include "Components/NTTD_HealthComponent.h"
#include "Components/CapsuleComponent.h"
#include "..\Public\NTTD_ZombieEnemy.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"

// Sets default values
ANTTD_ZombieEnemy::ANTTD_ZombieEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	MyHealthComponent = CreateDefaultSubobject<UNTTD_HealthComponent>(TEXT("MyHealthComponent"));

	RightHandCollider = CreateDefaultSubobject<UCapsuleComponent>(TEXT("RightHandCollider"));
	RightHandCollider->SetupAttachment(GetMesh(), RightHandAttackSocketName);
	RightHandCollider->SetCollisionResponseToAllChannels(ECR_Ignore);
	RightHandCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECR_Overlap);
	RightHandCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	LeftHandCollider = CreateDefaultSubobject<UCapsuleComponent>(TEXT("LeftHandCollider"));
	LeftHandCollider->SetupAttachment(GetMesh(), LeftHandAttackSocketName);
	LeftHandCollider->SetCollisionResponseToAllChannels(ECR_Ignore);
	LeftHandCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECR_Overlap);
	LeftHandCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	bIsHeavilyDamaged = false;
	HeavilyDamagedRatio = 0.3f;

	RightHandAttackSocketName = "RightHandAttackSocket";
	LeftHandAttackSocketName = "LeftHandAttackSocket";

}

// Called when the game starts or when spawned
void ANTTD_ZombieEnemy::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(MyHealthComponent))
	{
		MyHealthComponent->OnHealthUpdateDelegate.AddDynamic(this, &ANTTD_ZombieEnemy::CheckIfZombieIsHeavilyDamaged);
		MyHealthComponent->OnDeadDelegate.AddDynamic(this, &ANTTD_ZombieEnemy::Death);
	}

	/*
	if (IsValid(RightHandCollider))
	{

	}

	if (IsValid(LeftHandCollider))
	{

	}
	*/

	MyController = GetController();
	
}

void ANTTD_ZombieEnemy::CheckIfZombieIsHeavilyDamaged(float CurrentHealth, float MaxHealth)
{
	if (bIsHeavilyDamaged)
	{
		return;
	}
	
	float MinimumHealthToBeOkay = MaxHealth * HeavilyDamagedRatio;

	if (CurrentHealth <= MinimumHealthToBeOkay)
	{
		bIsHeavilyDamaged = true;
	}
}

void ANTTD_ZombieEnemy::Death(AActor* DamageCauser)
{
	GetMesh()->SetSimulatePhysics(false);
	GetCharacterMovement()->DisableMovement();

	if (IsValid(MyController))
	{
		MyController->UnPossess();
		MyController->Destroy();
	}
	//Delay and Kill
}

// Called every frame
void ANTTD_ZombieEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ANTTD_ZombieEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

