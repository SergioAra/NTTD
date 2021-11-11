// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterAnimInstance.h"
#include "NTTDCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

UCharacterAnimInstance::UCharacterAnimInstance():
	Speed(0.f),
	bIsInAir(false),
	bIsAccelerating(false),
	TIPCharacterYaw(0.f),
	TIPCharacterYawLastFrame(0.f),
	RootYawOffset(0.f),
	Pitch(0.f),
	bReloading(false),
	CharacterRotation(FRotator(0.f)),
	CharacterRotationLastFrame(FRotator(0.f)),
	bTurningInPlace(false)
{
	
}

void UCharacterAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if (NTTDCharacter == nullptr)
	{
		//try to get owner of instance each frame if NTTDCharacter is null
		NTTDCharacter = Cast<ANTTDCharacter>(TryGetPawnOwner());
	}
	if (NTTDCharacter)
	{
		//get lateral the speed of the character from velocity
		FVector Velocity{ NTTDCharacter->GetVelocity() };
		//moving in Z (falling/jumping/flying) wont affect movement animation
		Velocity.Z = 0;
		//get magnitude of vector
		Speed = Velocity.Size();

		//is the character in the air
		bIsInAir = NTTDCharacter->GetCharacterMovement()->IsFalling();

		//is the character accelerating
		bIsAccelerating = (NTTDCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f);

		
		FRotator AimRotation = NTTDCharacter->GetBaseAimRotation();
		FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(NTTDCharacter->GetVelocity());
		
	}
	
	//TurnInPlace();
}

void UCharacterAnimInstance::NativeInitializeAnimation()
{
	//get owner of instance and cast to NTTDCharacter class
	NTTDCharacter = Cast<ANTTDCharacter>(TryGetPawnOwner());
}

void UCharacterAnimInstance::TurnInPlace()
{
	if(NTTDCharacter == nullptr) return;

	Pitch = NTTDCharacter->GetBaseAimRotation().Pitch;
	
	
	if(Speed > 0 || bIsInAir)
	{
		//No turning in place since character is moving or is in the air
		bTurningInPlace = false;
		RootYawOffset = 0.f;
		TIPCharacterYaw = NTTDCharacter->GetActorRotation().Yaw;
		TIPCharacterYawLastFrame = TIPCharacterYaw;
		RotationCurveLastFrame = 0.f;
		RotationCurve = 0.f;
		
	}else
	{
		TIPCharacterYawLastFrame = TIPCharacterYaw;
		TIPCharacterYaw = NTTDCharacter->GetActorRotation().Yaw;

		//Delta between current yaw and last frame yaw
		const float TIPYawDelta{TIPCharacterYaw - TIPCharacterYawLastFrame};

		//root yaw offset updated and claped to [-180, 180]
		RootYawOffset = UKismetMathLibrary::NormalizeAxis(RootYawOffset - TIPYawDelta);

		//1.0 if turning, 0.0 of not
		const float Turning{GetCurveValue(TEXT("Turning"))};
		
		bTurningInPlace = Turning > 0;
		
		if(bTurningInPlace)
		{
			RotationCurveLastFrame = RotationCurve;
			RotationCurve = GetCurveValue(TEXT("Rotation"));
			const float DeltaRotation{RotationCurve - RotationCurveLastFrame};

			//if RootYawOffeset >0 ->Turning left, if RootYawOffset < 0 -> Turning right
			RootYawOffset > 0 ? RootYawOffset -= DeltaRotation : RootYawOffset += DeltaRotation;

			const float ABSRootYawOffset{FMath::Abs(RootYawOffset)};
			if(ABSRootYawOffset > 90)
			{
				const float YawExcess {ABSRootYawOffset - 90.f};
				RootYawOffset > 0 ? RootYawOffset -= YawExcess : RootYawOffset += YawExcess;
			}
			//if(GEngine) GEngine->AddOnScreenDebugMessage(1,-1,FColor::Blue, FString::Printf(TEXT("TIPCharacterYaw: %f"), TIPCharacterYaw));
			//if(GEngine) GEngine->AddOnScreenDebugMessage(2,-1,FColor::Red, FString::Printf(TEXT("RootYawOffset: %f"), RootYawOffset));
		}
		
	}
	
}


