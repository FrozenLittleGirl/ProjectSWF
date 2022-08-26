// Fill out your copyright notice in the Description page of Project Settings.

#include "GameFramework/ProjectileMovementComponent.h"
#include "Projectile.h"

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

int32 AProjectile::ReturnDirection() {
	int Direction = 1;
	if (GetActorRotation().Yaw != 0) {
		Direction = -1;
	}
	return Direction;
}

void AProjectile::LaunchProjectile(FVector Angle) {
	MovementComponent->SetVelocityInLocalSpace(Angle * LaunchSpeed);
	MovementComponent->Activate();
}

void AProjectile::AttachMovement(UProjectileMovementComponent* Component) {
	MovementComponent = Component;
}

