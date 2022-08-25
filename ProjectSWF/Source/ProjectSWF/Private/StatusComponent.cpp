// Fill out your copyright notice in the Description page of Project Settings.


#include "StatusComponent.h"

// Sets default values for this component's properties
UStatusComponent::UStatusComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UStatusComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UStatusComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UStatusComponent::TakeDamage(int32 Damage) {
	Health = Health - Damage;
	if (Health <= 0) {
		Died = true;
		DyingCount = FPlatformTime::Seconds();
	}
}

int32 UStatusComponent::ReturnHealth() {
	return Health;
}

bool UStatusComponent::DiedOrNot() {
	return Died;
}

float UStatusComponent::ReturnDyingCount() {
	return DyingCount;
}