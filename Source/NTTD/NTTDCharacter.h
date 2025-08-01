// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NTTDCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInfectionUpdateSignature, float, CurrentAmountOfInfection, float, MaxAmountOfInfection);

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
class ANTTD_GameMode;
class UAudioComponent;
class USoundCue;
UCLASS(Blueprintable)
class ANTTDCharacter : public ACharacter
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UNTTD_HealthComponent* MyHealthComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
	TSubclassOf<UDamageType> MyDamageType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UAudioComponent* VoiceSoundComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio")
	USoundCue* HurtSound;

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
	void PlayDryFireSound();
	void SendBullet();
	void PlayGunFireMontage();

	//Called internally to get the end location of a beam, used for shooting
	bool GetBeamEndLocation(const FVector& MuzzleSocketLocation, FVector& OutBeamLocation);

	//line trace for items under the crosshairs
	bool TraceUnderCrosshairs(FHitResult& Hit, FVector& OutHitLocation);

	void StartFireTimer();

	UFUNCTION()
	void AutoFireReset();

	//Checks to see if we have ammo of the equipped weapon ammo type
	bool CarryingAmmo();

	UFUNCTION(BlueprintCallable)
	void MakeDamage(AActor* OtherActor);
	
	void PickupAmmo(class AAmmo* Ammo);

	//Trace for items if OverlappedItemCount > 0
	void TraceForItems();

	void TraceForEnemy();

	UFUNCTION(BlueprintCallable)
	void FinishReloading();

	
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
	FORCEINLINE class UDecalComponent* GetCursorToWorld() const { return CursorToWorld; }

	FORCEINLINE AItem* GetTraceHitItem() const {return TraceHitItem;}

	FORCEINLINE bool GetBReloading() const {return bReloading;}

	FORCEINLINE int GetScore() const {return Score;}

	FORCEINLINE float GetInfectionRate() const {return InfectionRate;}

	FORCEINLINE bool BisDead() const {return bIsDead;}

	void SetInfectionRate(float NewInfectionRate) {InfectionRate = NewInfectionRate;}

	UFUNCTION(BlueprintCallable)
	void IncremetScore(int AmmounttoIncrease);
	
	//Adds / substracts to/from Overlapped ItemCount and updates bShouldTrace for items
	void IncrementOverlappedItemCount(int8 Amount);

	//Get picked up item
	void GetPickupItem(AItem* Item);

	//Called when the fire button is pressed
	void FireWeapon();

	//Handle reloading the weapon
	void ReloadWeapon();

	void InfectionGrowth();

	void SetCurrentAmountOfInfection(float NewAmountOfInfection);

	float GetCurrentAmountOfInfection() { return CurrentAmountOfInfection; };

	void UpdateInitialInfection();

	UFUNCTION()
	void Death(AActor* Killer);

	UFUNCTION()
	void ReceiveDamage(UNTTD_HealthComponent* HealthComponent, AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

	UNTTD_HealthComponent* GetHealthComponent() { return MyHealthComponent; };

public:
	//Delegates
	UPROPERTY(BlueprintAssignable)
	FOnInfectionUpdateSignature OnInfectionUpdateDelegate;

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

	//enemy traced last frame
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	class ANTTD_ZombieEnemy* TraceHitEnemyLastFrame;

	//enemy traced currently (could be null)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class ANTTD_ZombieEnemy* TraceHitEnemy;
	
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class USoundCue* DryFireSound;

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
	int32 AmmoCount;

	//Montage for reload animation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* ReloadMontage;

	bool bReloading;

	ANTTD_GameMode* MyGameModeReference;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Gameplay, meta = (AllowPrivateAccess = "true"))
	int Score;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	class USoundCue* AttackedSound;

	

protected:
	UPROPERTY(BlueprintReadOnly, Category = "GameOver")
	bool bIsDead;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Infection", meta = (ClampMin = 0.0f))
	float MaxInfection;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Infection", meta = (ClampMin = 0.0f))
	float InfectionRate;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Infection", meta = (ClampMin = 0.0f))
	float CurrentAmountOfInfection;

	FTimerHandle TimerHandle_InfectionGrowth;

	FTimerHandle TimerHandle_UpdateInfection;


	//----------------------------------------------------------------------
};

