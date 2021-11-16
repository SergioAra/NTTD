// Copyright Epic Games, Inc. All Rights Reserved.

#include "NTTDCharacter.h"

#include "Ammo.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Materials/Material.h"
#include "Engine/World.h"
#include "Item.h"
#include "NTTDPlayerController.h"
#include "Weapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "NTTD_HealthComponent.h"
#include "NTTD_ZombieEnemy.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "NTTD_GameMode.h"

ANTTDCharacter::ANTTDCharacter() :
	//item trace variables
	bShouldTraceForItems(false),
	OverlappedItemCount(0),

	//automatic gun fire rate (must be greater than ShootTimeDuration)
	AutomaticFireRate(0.2f),

	//ammo amount variable
	AmmoCount(30),

	bReloading(false),

	Score(0)




{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Create a decal in the world to show the cursor's location
	CursorToWorld = CreateDefaultSubobject<UDecalComponent>("CursorToWorld");
	CursorToWorld->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UMaterial> DecalMaterialAsset(TEXT("Material'/Game/TopDownCPP/Blueprints/M_Cursor_Decal.M_Cursor_Decal'"));
	if (DecalMaterialAsset.Succeeded())
	{
		CursorToWorld->SetDecalMaterial(DecalMaterialAsset.Object);
	}
	CursorToWorld->DecalSize = FVector(16.0f, 32.0f, 32.0f);
	CursorToWorld->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f).Quaternion());

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	MyHealthComponent = CreateDefaultSubobject<UNTTD_HealthComponent>(TEXT("MyHealthComponent"));

	MaxInfection = 100.0f;
	CurrentAmountOfInfection = 0.0f;
	InfectionRate = 5.0f;


	bIsDead = false;
}

void ANTTDCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

	if (CursorToWorld != nullptr)
	{
		if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
		{
			if (UWorld* World = GetWorld())
			{
				FHitResult HitResult;
				FCollisionQueryParams Params(NAME_None, FCollisionQueryParams::GetUnknownStatId());
				FVector StartLocation = TopDownCameraComponent->GetComponentLocation();
				FVector EndLocation = TopDownCameraComponent->GetComponentRotation().Vector() * 2000.0f;
				Params.AddIgnoredActor(this);
				World->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, Params);
				FQuat SurfaceRotation = HitResult.ImpactNormal.ToOrientationRotator().Quaternion();
				CursorToWorld->SetWorldLocationAndRotation(HitResult.Location, SurfaceRotation);
			}
		}
		else if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			FHitResult TraceHitResult;
			PC->GetHitResultUnderCursor(ECC_Visibility, true, TraceHitResult);
			FVector CursorFV = TraceHitResult.ImpactNormal;
			FRotator CursorR = CursorFV.Rotation();
			CursorToWorld->SetWorldLocation(TraceHitResult.Location);
			CursorToWorld->SetWorldRotation(CursorR);
			//Check OverlappedItemCount and trace for items if allowed 
			TraceForItems();
			TraceForEnemy();
		}
	}
}


void ANTTDCharacter::BeginPlay()
{
	Super::BeginPlay();

	//Spawn the default weapon and equip it
	EquipWeapon(SpawnDefaultWeapon());

	MyGameModeReference = Cast<ANTTD_GameMode>(GetWorld()->GetAuthGameMode());

	if (IsValid(MyHealthComponent))
	{
		MyHealthComponent->OnDeadDelegate.AddDynamic(this, &ANTTDCharacter::Death);
	}

	GetWorld()->GetTimerManager().SetTimer(TimerHandle_UpdateInfection, this, &ANTTDCharacter::UpdateInitialInfection, 0.2f, false);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_InfectionGrowth, this, &ANTTDCharacter::InfectionGrowth, 1.0f, true);
}


AWeapon* ANTTDCharacter::SpawnDefaultWeapon()
{
	//Check the TSUbclassOf variable
	if(DefaultWeaponClass)
	{
		// Spawn the weapon and return pointer
		return GetWorld()->SpawnActor<AWeapon>(DefaultWeaponClass);
	}
	return nullptr;
}

void ANTTDCharacter::EquipWeapon(AWeapon* WeaponToEquip)
{
	if(WeaponToEquip)
	{
		//Get hand socket
		const USkeletalMeshSocket* HandSocket = GetMesh()->GetSocketByName(FName("RightHandSocket"));
		
		if(HandSocket)
		{
			//Attach the weapon to the hand socket RightHandSocket
			HandSocket->AttachActor(WeaponToEquip, GetMesh());
		}
		//Set equipped weapon to the newly spawned default weapon 
		EquippedWeapon = WeaponToEquip;
		EquippedWeapon->SetItemState(EItemState::EIS_Equipped);
		
	}
}

bool ANTTDCharacter::WeaponHasAmmo()
{
	if(EquippedWeapon == nullptr) return false;

	return EquippedWeapon->GetAmmo() > 0;
}

void ANTTDCharacter::PlayFireSound()
{
	//Play the sound of the object:
	//if there is a valid sound pointer
	if (FireSound)
	{
		//play the sound in object
		UGameplayStatics::PlaySound2D(this,FireSound);
	}	
}

void ANTTDCharacter::SendBullet()
{
	//Send bullet:
	//find socket to attach particles
	const USkeletalMeshSocket* BarrelSocket = EquippedWeapon->GetItemMesh()->GetSocketByName("BarrelSocket");

	//if there is a valid socket
	if (BarrelSocket)
	{
		//find transform of socket
		const FTransform SocketTransform = BarrelSocket->GetSocketTransform(EquippedWeapon->GetItemMesh());

		//if there is a valid particle
		if (MuzzleFlash)
		{
			//spawn particles in socket
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, SocketTransform);
		}

		FVector BeamEnd;
		bool bBeamEnd = GetBeamEndLocation(SocketTransform.GetLocation(), BeamEnd);
		
		//spawn impact particles after updating BeamEndPoint
		if (ImpactParticles)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, BeamEnd);
		}

		if (BeamParticles)
		{
			UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BeamParticles, SocketTransform);
			if (Beam)
			{
				Beam->SetVectorParameter(FName("Target"), BeamEnd);
			}
		}
	}
}

void ANTTDCharacter::PlayGunFireMontage()
{
	//Play fire montage:
	//find anim instance from mesh
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	//if there is a valid anim instance and a valid montage
	if (AnimInstance && HipFireMontage)
	{
		//play montage
		AnimInstance->Montage_Play(HipFireMontage);
		//from selected section
		AnimInstance->Montage_JumpToSection(FName("StartFire"));
	}
}

bool ANTTDCharacter::GetBeamEndLocation(const FVector& MuzzleSocketLocation, FVector& OutBeamLocation)
{
	
	FHitResult Hit;
	bool bCrosshairHit =TraceUnderCrosshairs(Hit, OutBeamLocation);
	if(bCrosshairHit)
	{
		OutBeamLocation = FVector(Hit.ImpactPoint.X,Hit.ImpactPoint.Y,MuzzleSocketLocation.Z);
	}
	
	//Perform a second trace: from the gun barrel

	FHitResult WeaponTraceHit;
	const FVector WeaponTraceStart{ MuzzleSocketLocation };
	const FVector StartToEnd{OutBeamLocation - MuzzleSocketLocation};
	const FVector WeaponTraceEnd{ MuzzleSocketLocation + StartToEnd *300};
	GetWorld()->LineTraceSingleByChannel(WeaponTraceHit, WeaponTraceStart, WeaponTraceEnd, ECollisionChannel::ECC_Visibility);

	//object between barrel and BeamEndpoint?
	if (WeaponTraceHit.bBlockingHit)
	{
		OutBeamLocation = FVector(WeaponTraceHit.Location.X,WeaponTraceHit.Location.Y,MuzzleSocketLocation.Z);
		
		MakeDamage(WeaponTraceHit.Actor.Get());
		return true;
	}
	OutBeamLocation = FVector(Hit.ImpactPoint.X,Hit.ImpactPoint.Y,MuzzleSocketLocation.Z);
	return false;
}

bool ANTTDCharacter::TraceUnderCrosshairs(FHitResult& Hit, FVector& OutHitLocation)
{
	APlayerController* TraceController = Cast<APlayerController>(GetController());
	TraceController->GetHitResultUnderCursor(ECC_Visibility, false, Hit);
	if(Hit.bBlockingHit)
	{
		OutHitLocation = Hit.ImpactPoint;
		return true;
	}
	return false;
}

void ANTTDCharacter::StartFireTimer()
{
	GetWorldTimerManager().SetTimer(AutoFireTimer, this, &ANTTDCharacter::AutoFireReset, AutomaticFireRate);
}

void ANTTDCharacter::AutoFireReset()
{
	if(WeaponHasAmmo())
	{
		ANTTDPlayerController* const PlayerController = CastChecked<ANTTDPlayerController>(Controller);
		if (PlayerController->BMoveToMouseCursor() && PlayerController->BLockAim())
		{
			FireWeapon();
		}
	}else
	{
		ReloadWeapon();
	}
}

bool ANTTDCharacter::CarryingAmmo()
{
	if(EquippedWeapon == nullptr) return false;

		return AmmoCount > 0;
}

void ANTTDCharacter::MakeDamage(AActor* OtherActor)
{
	if (IsValid(OtherActor))
	{
		ANTTD_ZombieEnemy* PossibleEnemy = Cast<ANTTD_ZombieEnemy>(OtherActor);
		if (IsValid(PossibleEnemy))
		{
			UGameplayStatics::ApplyDamage(PossibleEnemy, EquippedWeapon->GetDamageToApply(), GetController(), this, MyDamageType);
		}
	}
}

void ANTTDCharacter::PickupAmmo(AAmmo* Ammo)
{

	AmmoCount += Ammo->GetItemCount();
	//check to see if the gun is empty
	if(EquippedWeapon->GetAmmo() == 0)
	{
		ReloadWeapon();
	}

	Ammo->Destroy();

}

void ANTTDCharacter::TraceForItems()
{
	
	if(bShouldTraceForItems)
	{
		FHitResult ItemTraceResult;
		FVector HitLocation;
		TraceUnderCrosshairs(ItemTraceResult, HitLocation);
		if(ItemTraceResult.bBlockingHit)
		{
			TraceHitItem = Cast<AItem>(ItemTraceResult.GetActor());
			if(TraceHitItem && TraceHitItem->IsPlayerOverlapping())
			{
				TraceHitItem->EnableCustomDepth();
			}
			
			//An AItem was hit last frame
			if(TraceHitItemLastFrame)
			{
				if(TraceHitItem != TraceHitItemLastFrame)
				{
					//we are hitting a different AItem this frame for last frame
					//or AItem is null this frame
					TraceHitItemLastFrame->DisableCustomDepth();
				}
			}
			//store a reference for hitItem last frame
			TraceHitItemLastFrame = TraceHitItem;
		}
	}else if(TraceHitItemLastFrame)
	{
		//no longer overlapping any items
		//item last frame should not show widget
		TraceHitItemLastFrame->DisableCustomDepth();
	}
}

void ANTTDCharacter::TraceForEnemy()
{
	FHitResult ItemTraceResult;
	FVector HitLocation;
	TraceUnderCrosshairs(ItemTraceResult, HitLocation);
	if(ItemTraceResult.bBlockingHit)
	{
		TraceHitEnemy = Cast<ANTTD_ZombieEnemy>(ItemTraceResult.GetActor());
		if(TraceHitEnemy)
		{
			TraceHitEnemy->EnableCustomDepth();
		}
			
		//An AItem was hit last frame
		if(TraceHitEnemyLastFrame)
		{
			if(TraceHitEnemy != TraceHitEnemyLastFrame)
			{
				//we are hitting a different AItem this frame for last frame
				//or AItem is null this frame
				TraceHitEnemyLastFrame->DisableCustomDepth();
			}
		}
		//store a reference for hitItem last frame
		TraceHitEnemyLastFrame = TraceHitEnemy;
	}

}

void ANTTDCharacter::DropWeapon()
{
	//check for inventory also (planned feature)
	if(EquippedWeapon)
	{
		FDetachmentTransformRules DetachmentTransformRules(EDetachmentRule::KeepWorld, true);
		EquippedWeapon->GetItemMesh()->DetachFromComponent(DetachmentTransformRules);

		EquippedWeapon->SetItemState(EItemState::EIS_Falling);
		EquippedWeapon->ThrowWeapon();
	}
}

void ANTTDCharacter::SwapWeapon(AWeapon* WeaponToSwap)
{
	DropWeapon();
	EquipWeapon(WeaponToSwap);
	TraceHitItem = nullptr;
	TraceHitItemLastFrame = nullptr;
}


void ANTTDCharacter::IncremetScore(int AmmounttoIncrease)
{
	Score += AmmounttoIncrease;
}

void ANTTDCharacter::IncrementOverlappedItemCount(int8 Amount)
{
	
	if(OverlappedItemCount + Amount <= 0)
	{
		OverlappedItemCount = 0;
		bShouldTraceForItems = false;
	}else
	{
		OverlappedItemCount += Amount;
		bShouldTraceForItems = true;
	}
}

void ANTTDCharacter::GetPickupItem(AItem* Item)
{
	if(Item->GetEquipSound())
	{
		UGameplayStatics::PlaySound2D(this, Item->GetEquipSound());
	}
	
	auto Weapon = Cast<AWeapon>(Item);
	if(Weapon)
	{
		SwapWeapon(Weapon);
	}
	
	auto Ammo = Cast<AAmmo>(Item);
    if(Ammo)
    {
    	PickupAmmo(Ammo);
    }
}

void ANTTDCharacter::FireWeapon()
{
	if(bIsDead) return;
	if(bReloading) return;
	if(EquippedWeapon == nullptr) return;
	if(WeaponHasAmmo())
	{
		PlayFireSound();
		SendBullet();
		PlayGunFireMontage();

		//subtract 1 from the weapon's ammo
		EquippedWeapon->DecrementAmmo();

		StartFireTimer();
	}
}

void ANTTDCharacter::ReloadWeapon()
{
	if(bIsDead) return;
	if(bReloading) return;
	if(EquippedWeapon == nullptr) return;

	// Do we have enough ammo of the correct type and the clip is not full
	if(CarryingAmmo() && !EquippedWeapon->ClipIsFull())
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if(ReloadMontage && AnimInstance)
		{
			bReloading = true;
			AnimInstance->Montage_Play(ReloadMontage);
			AnimInstance->Montage_JumpToSection(EquippedWeapon->GetReloadMontageSection());
		}
	}
}

void ANTTDCharacter::InfectionGrowth()
{
	if (CurrentAmountOfInfection < MaxInfection)
	{
		if(CurrentAmountOfInfection + InfectionRate < 0) return;
		
		CurrentAmountOfInfection += InfectionRate;
		OnInfectionUpdateDelegate.Broadcast(CurrentAmountOfInfection, MaxInfection);
	}
	else
	{
		OnInfectionUpdateDelegate.Broadcast(MaxInfection, MaxInfection);
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_InfectionGrowth);
		UGameplayStatics::ApplyDamage(this, MyHealthComponent->GetMaxHealth(), this->GetController(), this, MyDamageType);
	}
}

void ANTTDCharacter::SetCurrentAmountOfInfection(float NewAmountOfInfection)
{
	CurrentAmountOfInfection = NewAmountOfInfection;
	OnInfectionUpdateDelegate.Broadcast(CurrentAmountOfInfection, MaxInfection);

}

void ANTTDCharacter::UpdateInitialInfection()
{
	OnInfectionUpdateDelegate.Broadcast(CurrentAmountOfInfection, MaxInfection);
}

void ANTTDCharacter::Death(AActor* Killer)
{
	if (!bIsDead)
	{
		bIsDead = true;
		if (IsValid(MyGameModeReference))
		{
			
			MyGameModeReference->GameOver(this);
		}
	}
}


void ANTTDCharacter::FinishReloading()
{
	bReloading = false;
	if(EquippedWeapon == nullptr) return;
	
	//Space left in the magazine of EquippedWeapon when reloading
	const int32 MagEmptySpace = EquippedWeapon->GetMagazineCapacity() - EquippedWeapon->GetAmmo();

	if(MagEmptySpace > AmmoCount)
	{
		//Reload the magazine with all the ammo available
		EquippedWeapon->ReloadAmmo(AmmoCount);
		AmmoCount = 0;
	}else
		{
			//Reload the magazine with the required bullets to fill it
			EquippedWeapon->ReloadAmmo(MagEmptySpace);
			AmmoCount -= MagEmptySpace;
		}
}
