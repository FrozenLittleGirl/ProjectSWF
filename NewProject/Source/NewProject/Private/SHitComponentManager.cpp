// Fill out your copyright notice in the Description page of Project Settings.


#include "SHitComponentManager.h"

#include "SHitComponent.h"
#include "SHurtComponent.h"

DEFINE_LOG_CATEGORY(LogHitComponentManager);

// Sets default values for this component's properties
USHitComponentManager::USHitComponentManager()
: ShowDebugDisplay(false)
{
}


// Called when the game starts
void USHitComponentManager::BeginPlay()
{
	Super::BeginPlay();

	TArray<UActorComponent*> hitBoxes = GetOwner()->GetComponentsByClass(USHitComponent::StaticClass());
	for (UActorComponent* box : hitBoxes)
	{
		if (UPrimitiveComponent* hitComp = Cast<UPrimitiveComponent>(box))
		{
			hitComp->OnComponentBeginOverlap.AddDynamic(this, &USHitComponentManager::AttackBeginOverlap);
		}
	}
}

void USHitComponentManager::AttackBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherComp && !OtherComp->IsA<USHurtComponent>())
	{
		return;
	}
	
	// hitting itself
	if (!OtherActor || OtherActor == OverlappedComponent->GetOwner())
	{
		return;
	}

	for (FName tag : InvalidTargetGroup)
	{
		if (OtherActor->ActorHasTag(tag))
		{
			// invalid attack target
			return;
		}
	}

	if (USHitComponent* hitComp = Cast<USHitComponent>(OverlappedComponent))
	{
		hitComp->ApplyAttackEffects(OtherActor, OtherComp);
	}
	
	if (ShowDebugDisplay)
	{
		UE_LOG(LogHitComponentManager, Log, TEXT("%s attacked %s"), *OverlappedComponent->GetOwner()->GetName(), *OtherActor->GetName());
	}
}

