// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CharacterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class NTTD_API UCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UCharacterAnimInstance();

	//macro to make it callable from animation blueprint
	UFUNCTION(BlueprintCallable)
	//analog to tick function in actor
	void UpdateAnimationProperties(float DeltaTime);

	//override function for AnimInstance class, analog to the BeginPlay in actor
	virtual void NativeInitializeAnimation() override;

	protected:
	//Handle turning in place variables
	void TurnInPlace();

	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class ANTTDCharacter* NTTDCharacter;

	//speed of the character
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float Speed;

	//Whether or not the character is in the air
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bIsInAir;

	//whether or not the character is moving
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bIsAccelerating;

	//yaw of the character this frame for Turning In Place
	float TIPCharacterYaw;

	//Yaw of the character the previous frame for Turning In Place
	float TIPCharacterYawLastFrame;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	float RootYawOffset;
	
	//Rotation curve value this frame
	float RotationCurve;
	
	//Rotaiton curve value last frame
	float RotationCurveLastFrame;

	//The pitch of the aim rotation used for aim offset
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	float Pitch;

	//True when reloading, used to prevent aim offset while reloading
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	bool bReloading;

	//Current character yaw
	FRotator CharacterRotation;
	
	//Character yaw last frame
	FRotator CharacterRotationLastFrame;

	//True when turning in place
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	bool bTurningInPlace;
	
};
