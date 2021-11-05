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
UCLASS(Blueprintable)
class ANTTDCharacter : public ACharacter
{
	GENERATED_BODY()

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

	//----------------------------------------------------------------------
};

