// Fill out your copyright notice in the Description page of Project Settings.

#include <Runtime/Engine/Classes/Kismet/GameplayStatics.h>
#include "Components/CapsuleComponent.h"
#include "HitBoxActor.h"

// Sets default values
AHitBoxActor::AHitBoxActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AHitBoxActor::BeginPlay()
{
	Super::BeginPlay();
	

}

// Called every frame
void AHitBoxActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AHitBoxActor::StoreValues(int32 DamageAmount, float Time) {
	Damage = DamageAmount;
	PersistTime = Time;
}

void AHitBoxActor::AttachHitBox(UCapsuleComponent* Box) {
	HitBox = Box;
}

void AHitBoxActor::InitializeHitBox(FVector Location, FRotator Rotation, float Hight, float Radius) {
	HitBox->SetRelativeLocationAndRotation(Location, Rotation);
	HitBox->SetCapsuleSize(Radius, Hight, true);
}

void AHitBoxActor::InitializeValues(int32 DirectionTo, FVector ForceTo) {
	Force = ForceTo;
}

int32 AHitBoxActor::ReturnDirection() {
	int Direction = 1;
	if (GetActorRotation().Yaw != 0) {
		Direction = -1;
	}
	return Direction;
}