// Fill out your copyright notice in the Description page of Project Settings.


#include "SHurtComponent.h"

#include "SStatusComponent.h"

USHurtComponent::USHurtComponent()
{
}

void USHurtComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerStatusComp = Cast<USStatusComponent>(GetOwner()->GetComponentByClass(USStatusComponent::StaticClass()));
}

void USHurtComponent::TakeDamage(AActor* Instigator, float Damage)
{
	if (!ensureMsgf(OwnerStatusComp, TEXT("Target's StatusComp is null. Please attack a StatusComponent to the target.")))
	{
		return;
	}
	
	OwnerStatusComp->ApplyHealthChange(Instigator, Damage);
}
