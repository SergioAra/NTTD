// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NTTD_ZombieEnemy.generated.h"

class UAnimMontage;
class UNTTD_HealthComponent;
class AController;
class UCapsuleComponent;
class UAnimInstance;
class UWidgetComponent;
class UNTTD_ZombieHealthBar;
class AItem;

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UWidgetComponent* WidgetHealthBarComponent;

protected:
	//Variables

	UPROPERTY(BlueprintReadWrite, Category = "State")
	bool bIsHeavilyDamaged;

	UPROPERTY(BlueprintReadOnly, Category = "State")
	bool bIsDead;

	UPROPERTY(BlueprintReadWrite, Category = "Attack")
	bool bIsAttacking;

	bool bIsShowingHealthbar;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack", meta = (ClampMin = 0.0f))
	float DamageToApply;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "State", meta = (ClampMin = 0.1f, ClampMax = 1.0f))
	float HeavilyDamagedRatio;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loot", meta = (ClampMin = 0.0f, ClampMax = 1.0f))
	float LootProbability;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack")
	FName RightHandAttackSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack")
	FName LeftHandAttackSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
	TSubclassOf<UDamageType> MyDamageType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* NormalAttackRightArmMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* NormalAttackLeftArmMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* HeavilyDamagedAttackRightArmMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* HeavilyDamagedAttackLeftArmMontage;

	UPROPERTY(BlueprintReadOnly, Category = "AI")
	AController* MyController;

	UAnimInstance* MyAnimInstance;

	UNTTD_ZombieHealthBar* HealthbarReference;

	//Line trace collides with box to hit enemy
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
	class UBoxComponent* CollisionBox;

	FTimerHandle TimerHandle_HideHealthBar;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Loot")
	TSubclassOf<AItem> LootItemClass;

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

	UFUNCTION()
	void OnHealthChange(UNTTD_HealthComponent* CurrentHealthComponent, AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnHealthChange(UNTTD_HealthComponent* CurrentHealthComponent, AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION(BlueprintImplementableEvent)
	void BP_Death();

	UFUNCTION(BlueprintImplementableEvent)
	void BP_PlayMontage(UAnimMontage* MontageToPlay);

	void ShowHealthbar();

	void HideHealthbar();

	void SpawnLoot();

	virtual void InitializeCustomDepth();


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
	bool GetIsHeavilyDamaged() { return bIsHeavilyDamaged; };

	UFUNCTION(BlueprintCallable)
	void Attack();

	UFUNCTION(BlueprintCallable)
	void MakeDamage(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void SetRightHandColliderCollision(ECollisionEnabled::Type NewCollisionState);

	void SetLeftHandColliderCollision(ECollisionEnabled::Type NewCollisionState);

	virtual void EnableCustomDepth();
	virtual void DisableCustomDepth();

};
