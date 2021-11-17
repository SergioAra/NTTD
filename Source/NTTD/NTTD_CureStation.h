// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NTTDCharacter.h"
#include "GameFramework/Actor.h"
#include "NTTD_CureStation.generated.h"

UCLASS()
class NTTD_API ANTTD_CureStation : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANTTD_CureStation();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//called when overlapping AreaSphere, requires specific parameters to work with the component overlap
	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	//Cakked when end overlapping AreaSphere
	UFUNCTION()
	void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	virtual void InitializeCustomDepth();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Infection", meta = (ClampMin = 0.0f))
	float MaxCharge;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Infection", meta = (ClampMin = 0.0f))
	float CurrentRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Infection", meta = (ClampMin = 0.0f))
	float ChargeRate;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Infection", meta = (ClampMin = 0.0f))
	float DischargeRate;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Infection", meta = (ClampMin = 0.0f))
	float CurrentAmountOfCharge;

	FTimerHandle TimerHandle_ChargeGrowth;

	FTimerHandle TimerHandle_UpdateCharge;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
private:

	//Skeletal mesh for the item
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* ItemMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* SecondItemMesh;
	
	//Line trace collides with box to show HUD widgets
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* CollisionBox;
	
	//Enables item tracing when overlapped 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	class USphereComponent * AreaSphere;
	
	//is the player overlapping with the item 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	bool bIsPlayerOverlapping;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	class USoundCue* ChargedSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
    class USoundCue* DischargedSound;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
    class USoundCue* CureLoopSound;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	class UAudioComponent* AudioComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	class UAudioComponent* CureLoopComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	class ANTTDCharacter* CuredCharacter;

	void Recharge();

	void UpdateInitialCharge();

	bool bShouldCure;

	bool bChargeSoundPlayed;

	bool bDischargeSoundPlayed;

	void Cure();

	void StopCure();

	void Highlight();

	void HighlightSetOnCharge();

	void FullEnergyHighlight();

	void LowEnergyHighlight();

	void CureSwitch();

	void SpawnBeam();

	void CureSoundSwitch();

	void ChargeSoundSwitch();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* BeamParticles;

	
	
public:


	FORCEINLINE bool IsPlayerOverlapping() const {return bIsPlayerOverlapping;}

	FORCEINLINE USphereComponent* GetAreaSphere() const {return AreaSphere;}

	FORCEINLINE UBoxComponent* GetCollisionBox() const {return CollisionBox;}

	FORCEINLINE UStaticMeshComponent* GetItemMesh() const {return ItemMesh;}

	FORCEINLINE UStaticMeshComponent* GetSecondItemMesh() const {return SecondItemMesh;}

	virtual void EnableCustomDepth();
	virtual void DisableCustomDepth();
};
