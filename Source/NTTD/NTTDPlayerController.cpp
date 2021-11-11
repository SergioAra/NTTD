// Copyright Epic Games, Inc. All Rights Reserved.

#include "NTTDPlayerController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
//#include "HeadMountedDisplayFunctionLibrary.h"
#include "Item.h"
#include "NTTDCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Sound/SoundCue.h"
//#include "Engine/World.h"

ANTTDPlayerController::ANTTDPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
}

void ANTTDPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	FHitResult Hit;


	if(bLockAim)
	{
		if(TraceMouseCursor(Hit))
			SetNewRotateDestination(Hit.ImpactPoint);
	}else
		if(bMoveToMouseCursor)
		{
			if(TraceMouseCursor(Hit))
				SetNewMoveDestination(Hit.ImpactPoint);
		}
	
}

void ANTTDPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	InputComponent->BindAction("SetDestination", IE_Pressed, this, &ANTTDPlayerController::OnSetDestinationPressed);
	InputComponent->BindAction("SetDestination", IE_Released, this, &ANTTDPlayerController::OnSetDestinationReleased);
	
	InputComponent->BindAction("LockAim", IE_Pressed, this, &ANTTDPlayerController::OnLockAimPressed);
	InputComponent->BindAction("LockAim", IE_Released, this, &ANTTDPlayerController::OnLockAimReleased);

	InputComponent->BindAction("Select", IE_Pressed, this, &ANTTDPlayerController::SelectButtonPressed);
	InputComponent->BindAction("Select", IE_Released, this, &ANTTDPlayerController::SelectButtonReleased);

	InputComponent->BindAction("Reload", IE_Pressed, this, &ANTTDPlayerController::ReloadButtonPressed);
	InputComponent->BindAction("Reload", IE_Released, this, &ANTTDPlayerController::ReloadButtonReleased);

	// support touch devices 
	//InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &ANTTDPlayerController::MoveToTouchLocation);
	//InputComponent->BindTouch(EInputEvent::IE_Repeat, this, &ANTTDPlayerController::MoveToTouchLocation);

	//InputComponent->BindAction("ResetVR", IE_Pressed, this, &ANTTDPlayerController::OnResetVR);
}

/*
void ANTTDPlayerController::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}
*/

bool ANTTDPlayerController::TraceMouseCursor(FHitResult &Hit)
{
	// Trace to see what is under the mouse cursor
	GetHitResultUnderCursor(ECC_Visibility, false, Hit);

	if (Hit.bBlockingHit)
	{	
		return true;	
	}
	return false;
}

/*
void ANTTDPlayerController::MoveToMouseCursorVR()
{
	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
	{
		if (ANTTDCharacter* MyPawn = Cast<ANTTDCharacter>(GetPawn()))
		{
			if (MyPawn->GetCursorToWorld())
			{
				UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, MyPawn->GetCursorToWorld()->GetComponentLocation());
			}
		}
	}
	else
	{
		// Trace to see what is under the mouse cursor
		FHitResult Hit;
		GetHitResultUnderCursor(ECC_Visibility, false, Hit);

		if (Hit.bBlockingHit)
		{
			// We hit something, move there
			SetNewMoveDestination(Hit.ImpactPoint);
		}
	}
}
*/

/*
void ANTTDPlayerController::MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	FVector2D ScreenSpaceLocation(Location);

	// Trace to see what is under the touch location
	FHitResult HitResult;
	GetHitResultAtScreenPosition(ScreenSpaceLocation, CurrentClickTraceChannel, true, HitResult);
	if (HitResult.bBlockingHit)
	{
		// We hit something, move there
		SetNewMoveDestination(HitResult.ImpactPoint);
	}
}
*/

void ANTTDPlayerController::SetNewMoveDestination(const FVector DestLocation)
{
	APawn* const MyPawn = GetPawn();
	if (MyPawn)
	{
		float const Distance = FVector::Dist(DestLocation, MyPawn->GetActorLocation());

		// We need to issue move command only if far enough in order for walk animation to play correctly
		if ((Distance > 120.0f))
		{
			UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, DestLocation);
		}
	}
}

void ANTTDPlayerController::SetNewRotateDestination(const FVector DestLocation)
{
	APawn* const MyPawn = GetPawn();
	if (MyPawn)
	{
		// get the pawn location vector
		const FVector PawnLocation = MyPawn->GetActorLocation();
		// get the pawn rotation rotator
		const FRotator PawnRotation = MyPawn->GetControlRotation();
		// create a rotator with players Roll and Pitch, and the DestLocation's Yaw to rotate the player towards the cursor
		const FRotator Rotation = UKismetMathLibrary::MakeRotator(PawnRotation.Roll,PawnRotation.Pitch,UKismetMathLibrary::FindLookAtRotation(PawnLocation, DestLocation).Yaw);
		MyPawn->SetActorRotation(Rotation, ETeleportType::TeleportPhysics);
		// stop character movement if LockingAim while moving
		if(MyPawn->GetVelocity().Size() > 0)
			StopMovement();
	}
}

void ANTTDPlayerController::OnSetDestinationPressed()
{
	// set flag to keep updating destination until released
	bMoveToMouseCursor = true;
	if(bLockAim)
		FireWeapon();
}

void ANTTDPlayerController::OnSetDestinationReleased()
{
	// clear flag to indicate we should stop updating the destination
	bMoveToMouseCursor = false;
}

void ANTTDPlayerController::OnLockAimPressed()
{
	// set flag to lock player in place
	bLockAim = true;
}

void ANTTDPlayerController::OnLockAimReleased()
{
	//clear flag to allow player to move again
	bLockAim = false;
}

void ANTTDPlayerController::FireWeapon()
{
	ANTTDCharacter* const MyNTTDCharacter = Cast<ANTTDCharacter>(GetPawn()) ;
	if (MyNTTDCharacter)
	{
		MyNTTDCharacter->FireWeapon();
	}
}

void ANTTDPlayerController::SelectButtonPressed()
{
	ANTTDCharacter* const MyNTTDCharacter = Cast<ANTTDCharacter>(GetPawn());
	if(MyNTTDCharacter)
	{
		//check if traced item is valid and player is overlapping the AreaSphere to pickup the item, prevents picking up an item that is not overlapped
		AItem* TraceHitItem = MyNTTDCharacter->GetTraceHitItem();
		if(TraceHitItem && TraceHitItem->IsPlayerOverlapping())
		{
			MyNTTDCharacter->GetPickupItem(TraceHitItem);
			
		}
	}

}

void ANTTDPlayerController::SelectButtonReleased()
{
}

void ANTTDPlayerController::ReloadButtonPressed()
{
	ANTTDCharacter* const MyNTTDCharacter = Cast<ANTTDCharacter>(GetPawn());
	if(MyNTTDCharacter)
	{
		MyNTTDCharacter->ReloadWeapon();
	}
	
}

void ANTTDPlayerController::ReloadButtonReleased()
{
}
