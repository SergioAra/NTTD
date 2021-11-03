// Copyright Epic Games, Inc. All Rights Reserved.

#include "NTTDPlayerController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
//#include "HeadMountedDisplayFunctionLibrary.h"
#include "NTTDCharacter.h"
#include "Kismet/KismetMathLibrary.h"
//#include "Engine/World.h"

ANTTDPlayerController::ANTTDPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
}

void ANTTDPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	if(bLockAim || bMoveToMouseCursor)
	{
		TraceMouseCursor();
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

void ANTTDPlayerController::TraceMouseCursor()
{
	// Trace to see what is under the mouse cursor
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_Visibility, false, Hit);

	if (Hit.bBlockingHit)
	{
		if(bLockAim)
		{
			SetNewRotateDestination(Hit.ImpactPoint);
		}else
			if(bMoveToMouseCursor)
			{
				SetNewMoveDestination(Hit.ImpactPoint);
			}
		
	}
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
		FVector PawnLocation = MyPawn->GetActorLocation();
		FRotator PawnRotation = MyPawn->GetControlRotation();
		FRotator Rotation = UKismetMathLibrary::MakeRotator(PawnRotation.Roll,PawnRotation.Pitch,UKismetMathLibrary::FindLookAtRotation(PawnLocation, DestLocation).Yaw);
		MyPawn->SetActorRotation(Rotation, ETeleportType::TeleportPhysics);
	}
}

void ANTTDPlayerController::OnSetDestinationPressed()
{
	// set flag to keep updating destination until released
	bMoveToMouseCursor = true;
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
