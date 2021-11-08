// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NTTD_HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FOnHealthChangeSignature, UNTTD_HealthComponent*, HealthComponent, AActor*, DamagedActor, float, Damage, const UDamageType*, DamageType, AController*, InstigatedBy, AActor*, DamageCauser);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthUpdateSignature, float, CurrentHealth, float, MaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeadSignature, AActor*, DamageCauser);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NTTD_API UNTTD_HealthComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	//Variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float MaxHealth;

	UPROPERTY(BlueprintReadWrite, Category = "Settings")
	float CurrentHealth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings")
	bool bIsDead;

	UPROPERTY(BlueprintReadOnly, Category = "Settings")
	AActor* MyOwner;

	FTimerHandle TimerHanlde_UpdateInitialHealth;

public:
	//Delegates
	UPROPERTY(BlueprintAssignable)
	FOnHealthChangeSignature OnHealthChangeDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnHealthUpdateSignature OnHealthUpdateDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnDeadSignature OnDeadDelegate;

public:	
	// Sets default values for this component's properties
	UNTTD_HealthComponent();

	UFUNCTION(BlueprintCallable, Category = "State")
	bool GetIsDead() const { return bIsDead; };

	float GetCurrentHealth() { return CurrentHealth; };

	float GetMaxHealth() { return MaxHealth; };

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION()
	void TakingDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	void UpdateInitialHealth();


		
};
