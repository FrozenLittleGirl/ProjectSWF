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

int32 AHitBoxActor::ReturnDirection() {
	int Direction = 1;
	if (GetActorRotation().Yaw != 0) {
		Direction = -1;
	}
	return Direction;
}