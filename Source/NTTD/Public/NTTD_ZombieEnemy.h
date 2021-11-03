// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NTTD_ZombieEnemy.generated.h"

class UAnimMontage;
class UNTTD_HealthComponent;
class AController;
class UCapsuleComponent;

UCLASS()
class NTTD_API ANTTD_ZombieEnemy : public ACharacter
{
	GENERATED_BODY()

protected:
	//Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UNTTD_HealthComponent* MyHealthComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCapsuleComponent* RightHandCollider;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCapsuleComponent* LeftHandCollider;

protected:
	//Variables

	UPROPERTY(BlueprintReadOnly, Category = "State")
	bool bIsHeavilyDamaged;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "State", meta = (ClampMin = 0.1f, ClampMax = 1.0f))
	float HeavilyDamagedRatio;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack")
	FName RightHandAttackSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack")
	FName LeftHandAttackSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* NormalAttackRightArm;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* NormalAttackLeftArm;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* HeavilyDamagedAttackRightArm;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* HeavilyDamagedAttackLeftArm;

	UPROPERTY(BlueprintReadOnly, Category = "AI")
	AController* MyController;

public:
	// Sets default values for this character's properties
	ANTTD_ZombieEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void CheckIfZombieIsHeavilyDamaged(float CurrentHealth, float MaxHealth);

	UFUNCTION()
	void Death(AActor* DamageCauser);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
	bool GetIsHeavilyDamaged() { return bIsHeavilyDamaged; };

};
