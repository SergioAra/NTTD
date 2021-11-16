// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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

	//Item count
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	int32 ItemCount;
	
	//is the player overlapping with the item 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	bool bIsPlayerOverlapping;

	//Pointer to the character
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	class ANTTDCharacter* Character;

	//Sound played when item is equipped 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	class USoundCue* EquipSound;

	
	
public:


	FORCEINLINE bool IsPlayerOverlapping() const {return bIsPlayerOverlapping;}

	FORCEINLINE USphereComponent* GetAreaSphere() const {return AreaSphere;}

	FORCEINLINE UBoxComponent* GetCollisionBox() const {return CollisionBox;}

	FORCEINLINE UStaticMeshComponent* GetItemMesh() const {return ItemMesh;}

	FORCEINLINE UStaticMeshComponent* GetSecondItemMesh() const {return SecondItemMesh;}

	FORCEINLINE USoundCue* GetEquipSound() const {return EquipSound;}
	
	FORCEINLINE int32 GetItemCount() const {return ItemCount;}

	virtual void EnableCustomDepth();
	virtual void DisableCustomDepth();
};
