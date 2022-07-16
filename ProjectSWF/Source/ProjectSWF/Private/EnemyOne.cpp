// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyOne.h"
#include "../Public/StatusComponent.h"

// Sets default values
AEnemyOne::AEnemyOne()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Status = CreateDefaultSubobject<UStatusComponent>(TEXT("Status"));
	Status->SetupVariables(false, Health, BasicDamage);
}

// Called when the game starts or when spawned
void AEnemyOne::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemyOne::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemyOne::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemyOne::AttachCollision(UCapsuleComponent* CollisionBox) {
	HurtBox = CollisionBox;
}