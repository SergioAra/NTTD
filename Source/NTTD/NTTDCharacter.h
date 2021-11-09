// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NTTDCharacter.generated.h"


UENUM(BlueprintType)
enum class ECombatState: uint8
{
	ECS_Unoccupied UMETA(DisplayName = "Unoccupied"),
	ECS_FireTimerInProgress UMETA(DisplayName = "FireTimerInProgress"),
	ECS_Reloading UMETA(DisplayName = "Reloading"),
	
	ECS_MAX UMETA(DisplayName = "Default Max"),
	
};

class AWeapon;// delete later
class AItem; //delete later
class UNTTD_HealthComponent;
UCLASS(Blueprintable)
class ANTTDCharacter : public ACharacter
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UNTTD_HealthComponent* MyHealthComponent;

protected:

	//----------------------------------------------------------------------

	//Spawns the default weapon selected in the blueprints and equips it
	class AWeapon* SpawnDefaultWeapon();
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//drops equiped weapon and quips tracehititem
	void SwapWeapon(AWeapon* WeaponToSwap);

	//Detach weapon and let it fall to the ground
	void DropWeapon();

	//takes a weapon and attaches it to the mesh
	void EquipWeapon(class AWeapon* WeaponToEquip);

	//check to make sure our weapon has ammo
	bool WeaponHasAmmo();

	//FireWeapon functions
	void PlayFireSound();
	void SendBullet();
	void PlayGunFireMontage();

	//Called internally to get the end location of a beam, used for shooting
	bool GetBeamEndLocation(const FVector& MuzzleSocketLocation, FVector& OutBeamLocation);

	//line trace for items under the crosshairs
	bool TraceUnderCrosshairs(FHitResult& OutHitResult, FVector& OutHitLocation);

	void StartFireTimer();

	UFUNCTION()
	void AutoFireReset();

	//Checks to see if we have ammo of the equipped weapon ammo type
	bool CarryingAmmo();
	
public:

	//----------------------------------------------------------------------
	
	ANTTDCharacter();

	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns CursorToWorld subobject **/
	FORCEINLINE class UDecalComponent* GetCursorToWorld() { return CursorToWorld; }
	
	//Adds / substracts to/from Overlapped ItemCount and updates bShouldTrace for items
	void IncrementOverlappedItemCount(int8 Amount);

	//Get picked up item
	void GetPickupItem(AItem* Item);

	//Called when the fire button is pressed
	void FireWeapon();

	//Handle reloading the weapon
	void ReloadWeapon();



private:

	//true if there should be a trace every frame
	bool bShouldTraceForItems;
	
	//Number of AItems overlapped
	int8 OverlappedItemCount;
	
	//item traced last frame
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	class AItem* TraceHitItemLastFrame;

	//item traced currently (could be null)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class AItem* TraceHitItem;
	
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** A decal that projects to the cursor location. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UDecalComponent* CursorToWorld;

	//Currently equipped weapon
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class AWeapon* EquippedWeapon;

	//Set this in blueprints for the default weapon class
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true") )
	TSubclassOf<AWeapon> DefaultWeaponClass;

	//randomized gunshot sound cue
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class USoundCue* FireSound;

	//flash spawned at barrel socket
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* MuzzleFlash;

	//particles spawned upon bullet impact
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* ImpactParticles;

	//smoke trail for bullets
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* BeamParticles;

	//Montage for firing the weapon
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* HipFireMontage;

	//sets a timer between automatic gunshots
	FTimerHandle AutoFireTimer;
	
	//rate of fire
	float AutomaticFireRate;

	//map to keep track of ammo of the different ammo types
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	int32 Ammo;

	//Montage for reload animation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* ReloadMontage;


	//----------------------------------------------------------------------
};

