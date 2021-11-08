// Fill out your copyright notice in the Description page of Project Settings.


#include "NTTD_ANSMeleeRightArm.h"
#include "NTTD_ZombieEnemy.h"

void UNTTD_ANSMeleeRightArm::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	AActor* CharacterActor = MeshComp->GetOwner();
	if (IsValid(CharacterActor))
	{
		ANTTD_ZombieEnemy* Zombie = Cast<ANTTD_ZombieEnemy>(CharacterActor);
		if (IsValid(Zombie))
		{
			Zombie->SetRightHandColliderCollision(ECollisionEnabled::QueryOnly);
		}
	}
}

void UNTTD_ANSMeleeRightArm::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	AActor* CharacterActor = MeshComp->GetOwner();
	if (IsValid(CharacterActor))
	{
		ANTTD_ZombieEnemy* Zombie = Cast<ANTTD_ZombieEnemy>(CharacterActor);
		if (IsValid(Zombie))
		{
			Zombie->SetRightHandColliderCollision(ECollisionEnabled::NoCollision);
		}
	}
}
