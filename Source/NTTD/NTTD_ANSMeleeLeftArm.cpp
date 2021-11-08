// Fill out your copyright notice in the Description page of Project Settings.


#include "NTTD_ANSMeleeLeftArm.h"
#include "NTTD_ZombieEnemy.h"

void UNTTD_ANSMeleeLeftArm::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	AActor* CharacterActor = MeshComp->GetOwner();
	if (IsValid(CharacterActor))
	{
		ANTTD_ZombieEnemy* Zombie = Cast<ANTTD_ZombieEnemy>(CharacterActor);
		if (IsValid(Zombie))
		{
			Zombie->SetLeftHandColliderCollision(ECollisionEnabled::QueryOnly);
		}
	}
}

void UNTTD_ANSMeleeLeftArm::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	AActor* CharacterActor = MeshComp->GetOwner();
	if (IsValid(CharacterActor))
	{
		ANTTD_ZombieEnemy* Zombie = Cast<ANTTD_ZombieEnemy>(CharacterActor);
		if (IsValid(Zombie))
		{
			Zombie->SetLeftHandColliderCollision(ECollisionEnabled::NoCollision);
		}
	}
}
