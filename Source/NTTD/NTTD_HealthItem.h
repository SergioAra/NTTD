// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NTTD_HealthItem.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class USoundCue;

UCLASS()
class NTTD_API ANTTD_HealthItem : public AActor
{
	GENERATED_BODY()

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* MainColliderComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* HealthBoxMeshComponent;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HealthValue")
	float HealthValue;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio")
	USoundCue* HealSound;

	
public:	
	// Sets default values for this actor's properties
	ANTTD_HealthItem();

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void PlayHealSound();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
