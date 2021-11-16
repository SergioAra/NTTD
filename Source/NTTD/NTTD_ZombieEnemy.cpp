// Fill out your copyright notice in the Description page of Project Settings.


#include "NTTD_ZombieEnemy.h"

#include "Ammo.h"
#include "NTTD_HealthComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NTTDCharacter.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Controller.h"
#include "Components/WidgetComponent.h"
#include "NTTD_ZombieHealthBar.h"
#include "Item.h"

// Sets default values
ANTTD_ZombieEnemy::ANTTD_ZombieEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	RightHandAttackSocketName = "RightHandAttackSocket";
	LeftHandAttackSocketName = "LeftHandAttackSocket";

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

	WidgetHealthBarComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetHealthBarComponent"));
	WidgetHealthBarComponent->SetupAttachment(RootComponent);
	WidgetHealthBarComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetupAttachment(GetMesh());
	//block collision to all channels
	CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	//set collision to block visibility channel
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	bIsHeavilyDamaged = false;
	bIsAttacking = false;
	bIsShowingHealthbar = false;
	HeavilyDamagedRatio = 0.3f;
	DamageToApply = 25.0f;
	LootProbability = 0.75;
	ScoreValue = 50;
	AmmoCount = 5;

}

// Called when the game starts or when spawned
void ANTTD_ZombieEnemy::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(MyHealthComponent))
	{
		MyHealthComponent->OnHealthUpdateDelegate.AddDynamic(this, &ANTTD_ZombieEnemy::CheckIfZombieIsHeavilyDamaged);
		MyHealthComponent->OnDeadDelegate.AddDynamic(this, &ANTTD_ZombieEnemy::Death);
		MyHealthComponent->OnHealthChangeDelegate.AddDynamic(this, &ANTTD_ZombieEnemy::OnHealthChange);
	}

	if (IsValid(RightHandCollider))
	{
		RightHandCollider->OnComponentBeginOverlap.AddDynamic(this, &ANTTD_ZombieEnemy::MakeDamage);
	}

	if (IsValid(LeftHandCollider))
	{
		LeftHandCollider->OnComponentBeginOverlap.AddDynamic(this, &ANTTD_ZombieEnemy::MakeDamage);
	}

	MyController = GetController();

	if (IsValid(GetMesh()))
	{
		MyAnimInstance = GetMesh()->GetAnimInstance();
	}

	if (IsValid(WidgetHealthBarComponent->GetUserWidgetObject()))
	{
		HealthbarReference = Cast<UNTTD_ZombieHealthBar>(WidgetHealthBarComponent->GetUserWidgetObject());
		if (IsValid(HealthbarReference) && IsValid(MyHealthComponent))
		{
			MyHealthComponent->OnHealthUpdateDelegate.AddDynamic(HealthbarReference, &UNTTD_ZombieHealthBar::UpdateHealth);
			HideHealthbar();
		}
	}
	//Set custom depth to disabled
	InitializeCustomDepth();
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
	if (!bIsDead)
	{
		bIsDead = true;

		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		LeftHandCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		RightHandCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		SpawnLoot();

		BP_Death();
	}
}

void ANTTD_ZombieEnemy::OnHealthChange(UNTTD_HealthComponent* CurrentHealthComponent, AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	BP_OnHealthChange(CurrentHealthComponent, DamagedActor, Damage, DamageType, InstigatedBy, DamageCauser);

	if (CurrentHealthComponent->GetIsDead())
	{
		if (IsValid(HealthbarReference))
		{
			HideHealthbar();
		}
	}
	else
	{
		if (bIsShowingHealthbar)
		{
			GetWorld()->GetTimerManager().ClearTimer(TimerHandle_HideHealthBar);
		}
		else
		{
			ShowHealthbar();
			GetWorld()->GetTimerManager().SetTimer(TimerHandle_HideHealthBar, this, &ANTTD_ZombieEnemy::HideHealthbar, 1.0f, false);
		}
	}
}

void ANTTD_ZombieEnemy::ShowHealthbar()
{
	bIsShowingHealthbar = true;
	HealthbarReference->SetVisibility(ESlateVisibility::Visible);
}

void ANTTD_ZombieEnemy::HideHealthbar()
{
	bIsShowingHealthbar = false;
	HealthbarReference->SetVisibility(ESlateVisibility::Hidden);
}

void ANTTD_ZombieEnemy::SpawnLoot()
{
	if (!IsValid(LootItemClass))
	{
		return;
	}

	const float SelectedProbability = FMath::FRandRange(0, 1);

	if (SelectedProbability <= LootProbability)
	{
		FActorSpawnParameters SpawnParameter;
		SpawnParameter.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		AAmmo* Spawned = Cast<AAmmo>(GetWorld()->SpawnActor<AItem>(LootItemClass, GetActorLocation(), FRotator::ZeroRotator, SpawnParameter));
		if(Spawned)
		{
			Spawned->SetItemCount(AmmoCount);
		}
	}
}

void ANTTD_ZombieEnemy::InitializeCustomDepth()
{
	DisableCustomDepth();
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

void ANTTD_ZombieEnemy::Attack()
{
	//Behavior is in Blueprints
}

void ANTTD_ZombieEnemy::MakeDamage(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IsValid(OtherActor))
	{
		ANTTDCharacter* PossiblePlayer = Cast<ANTTDCharacter>(OtherActor);
		if (IsValid(PossiblePlayer))
		{
			UGameplayStatics::ApplyDamage(PossiblePlayer, DamageToApply, MyController, this, MyDamageType);
		}
	}
}

void ANTTD_ZombieEnemy::SetRightHandColliderCollision(ECollisionEnabled::Type NewCollisionState)
{
	RightHandCollider->SetCollisionEnabled(NewCollisionState);
}

void ANTTD_ZombieEnemy::SetLeftHandColliderCollision(ECollisionEnabled::Type NewCollisionState)
{
	LeftHandCollider->SetCollisionEnabled(NewCollisionState);
}

void ANTTD_ZombieEnemy::EnableCustomDepth()
{
	GetMesh()->SetRenderCustomDepth(true);
}

void ANTTD_ZombieEnemy::DisableCustomDepth()
{
	GetMesh()->SetRenderCustomDepth(false);
}

