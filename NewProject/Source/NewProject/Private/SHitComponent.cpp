// Fill out your copyright notice in the Description page of Project Settings.


#include "SHitComponent.h"

#include "SHurtComponent.h"

USHitComponent::USHitComponent()
: BasicAttackCollisionType(ECollisionEnabled::QueryOnly)
, Damage(-1.0f)
{
}

void USHitComponent::EnableCollision()
{
	SetCollisionEnabled(BasicAttackCollisionType);
}

void USHitComponent::DisableCollision()
{
	SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void USHitComponent::ApplyAttackEffects(AActor* OtherActor, UPrimitiveComponent* OtherComp)
{
	if (USHurtComponent* otherHurtComp = Cast<USHurtComponent>(OtherComp))
	{
		otherHurtComp->TakeDamage(OtherActor, Damage);
	}	
}




