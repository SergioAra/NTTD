// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Weapon.generated.h"

/**
 * 
 */
UCLASS()
class NTTD_API AWeapon : public AItem
{
	GENERATED_BODY()

public:
	
	//adds an impulse to the weapon
	void ThrowWeapon();

	FORCEINLINE int32 GetAmmo() const {return Ammo;}
	FORCEINLINE int32 GetMagazineCapacity() const {return MagazineCapacity;}

	//Called from character class when firing weapon
	void DecrementAmmo();
	
	FORCEINLINE FName GetReloadMontageSection() const {return ReloadMontageSection;}
	FORCEINLINE FName GetClipBoneName() const {return ClipBoneName;}

	void ReloadAmmo(int32 Amount);

	FORCEINLINE void SetMovingClip(bool Move) {bMovingClip = Move;}
	FORCEINLINE bool GetMovingClip() const {return bMovingClip;}

	bool ClipIsFull();




	//----------------------------------------------------------------------
	AWeapon();

	virtual void Tick(float DeltaTime) override;
protected:

	//----------------------------------------------------------------------
	void StopFalling();
	
private:

	FTimerHandle ThrowWeaponTimer;
	float ThrowWeaponTime;
	bool bFalling;

	//Ammo count for this weapon
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	int32 Ammo;

	//Maximum ammo the weapon can hold
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	int32 MagazineCapacity;

	//FName of the ReloadMontage section
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	FName ReloadMontageSection;

	//true when moving clip
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	bool bMovingClip;

	//Name for the clip bone
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	FName ClipBoneName;
	
	//----------------------------------------------------------------------
	
	
};
