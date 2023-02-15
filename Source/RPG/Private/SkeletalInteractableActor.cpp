// Fill out your copyright notice in the Description page of Project Settings.


#include "SkeletalInteractableActor.h"
#include "Components/SkeletalMeshComponent.h"

ASkeletalInteractableActor::ASkeletalInteractableActor() : Super()
{
	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));

	MeshComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
}

bool ASkeletalInteractableActor::Interact_Implementation(AActor* Interactor)
{
	if (Super::Interact_Implementation(Interactor))
	{
		UAnimInstance* AnimInstance = MeshComponent->GetAnimInstance();

		if (AnimInstance && OnInteractMontage)
			AnimInstance->Montage_Play(OnInteractMontage);
		return true;
	}
	else
	{
		return false;
	}
}


