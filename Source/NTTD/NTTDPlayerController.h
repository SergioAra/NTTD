// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "NTTDPlayerController.generated.h"

UCLASS()
class ANTTDPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ANTTDPlayerController();
	
	FORCEINLINE uint32 BMoveToMouseCursor() const {return bMoveToMouseCursor;}
	FORCEINLINE uint32 BLockAim() const {return bLockAim;}

protected:
	/** True if the controlled character should navigate to the mouse cursor. */
	uint32 bMoveToMouseCursor : 1;

	/** True if the controlled character should lock in place to aim. */
	uint32 bLockAim : 1;

	// Begin PlayerController interface
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;
	// End PlayerController interface

	/** Resets HMD orientation in VR. */
	//void OnResetVR();

	/** trace the current mouse cursor location vector. */
	bool TraceMouseCursor(FHitResult &Hit);

	/** Navigate player to the current mouse cursor location with VR option. */
	//void MoveToMouseCursorVR();

	/** Navigate player to the current touch location. */
	//void MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location);
	
	/** Navigate player to the given world location. */
	void SetNewMoveDestination(const FVector DestLocation);

	/** Rotate player to the given world location. */
	void SetNewRotateDestination(const FVector DestLocation);

	/** Input handlers for SetDestination action. */
	void OnSetDestinationPressed();
	void OnSetDestinationReleased();

	/** Input handlers for LockAim action. */
	void OnLockAimPressed();
	void OnLockAimReleased();

	void FireWeapon();
};


