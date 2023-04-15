// Fill out your copyright notice in the Description page of Project Settings.


#include "SStatusComponent.h"

DEFINE_LOG_CATEGORY(LogStatusComponent);

// Sets default values for this component's properties
USStatusComponent::USStatusComponent()
: ShowDebugDisplay(false)
, MaxHealth(10.0f)
{
	CurrentHealth = MaxHealth;
}


// Called when the game starts
void USStatusComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

bool USStatusComponent::ApplyHealthChange(AActor* Instigator, float Delta)
{
	CurrentHealth += Delta;
	OnHealthChanged.Broadcast(Instigator, this, CurrentHealth, Delta);

	if (ShowDebugDisplay)
	{
		UE_LOG(LogStatusComponent, Log, TEXT("%s is taking damage. Current Health: %d"), *GetOwner()->GetName(), CurrentHealth);
	}
	
	return true;
}

bool USStatusComponent::IsAlive() const
{
	return CurrentHealth > 0.0f;
}

float USStatusComponent::GetCurrentHealth()
{
	return CurrentHealth;
}

