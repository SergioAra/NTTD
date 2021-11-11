// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
AWeapon::AWeapon():
	ThrowWeaponTime(0.7f),
	bFalling(false),
	Ammo(30),
	MagazineCapacity(30),
	ReloadMontageSection(TEXT("Reload SMG")),
	ClipBoneName(TEXT("smg_clip")),
	DamageToApply(25.f)
{
	//Allows actor to tick
	PrimaryActorTick.bCanEverTick = true;
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//keep the weapon upright
	if(GetItemState() == EItemState::EIS_Falling && bFalling)
	{
		const FRotator MeshRotation {0.f, GetItemMesh()->GetComponentRotation().Yaw, 0.f};
		GetItemMesh()->SetWorldRotation(MeshRotation, false, nullptr, ETeleportType::TeleportPhysics);
	}
}

void AWeapon::ThrowWeapon()
{
	FRotator MeshRotation{0.f, GetItemMesh()->GetComponentRotation().Yaw, 0.f};
	GetItemMesh()->SetWorldRotation(MeshRotation, false, nullptr, ETeleportType::TeleportPhysics);

	//forward vector of the mesh
	const FVector MeshForward{GetItemMesh()->GetForwardVector()};
	//right vector of the mesh
	const FVector MeshRight {GetItemMesh()->GetRightVector()};
	//Andgle in which we throw the weapon, turns the Forward vector of the mesh on its right vector
	FVector ImpulseDirection = MeshRight.RotateAngleAxis(-20.f, MeshForward);
	//FMath::RandRange(0.f,90.f) for randomized throw
	//Direction in which we throw the weapon, turns the Z axis on the resulting vector of ImpulseRotation
	ImpulseDirection = ImpulseDirection.RotateAngleAxis(30.f, FVector(0.f,0.f,1.f));
	ImpulseDirection *= 10000.f;
	GetItemMesh()->AddImpulse(ImpulseDirection);

	bFalling = true;
	GetWorldTimerManager().SetTimer(ThrowWeaponTimer, this, &AWeapon::StopFalling, ThrowWeaponTime);
}

void AWeapon::DecrementAmmo()
{
	if(Ammo - 1 <= 0)
	{
		Ammo = 0;
	}else
	{
		--Ammo;
	}
}

void AWeapon::ReloadAmmo(int32 Amount)
{
	checkf(Ammo + Amount <= MagazineCapacity, TEXT("Attemped to reload with more than the magazine capacity"));
	Ammo += Amount;
}

bool AWeapon::ClipIsFull()
{
	return Ammo >= MagazineCapacity;
}

void AWeapon::StopFalling()
{
	bFalling = false;
	SetItemState(EItemState::EIS_Pickup);
}

