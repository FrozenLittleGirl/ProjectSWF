// Fill out your copyright notice in the Description page of Project Settings.

#include "../Public/StatusComponent.h"
#include "EnemyOne.h"

// Sets default values
AEnemyOne::AEnemyOne()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Status = CreateDefaultSubobject<UStatusComponent>(TEXT("Status"));
	Status->SetupVariables(false, Health, BasicDamage);
}

// Called when the game starts or when spawned
void AEnemyOne::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("%s possessing"), *(GetController()->GetName()));
}

// Called every frame
void AEnemyOne::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEnemyOne::TakingDamage() {
	UE_LOG(LogTemp, Warning, TEXT("%s is taking damage"), *(GetName()));
}

bool AEnemyOne::ReturnPlayerSpotted() {
	return PlayerSpotted;
}

void AEnemyOne::Walk() {
	if (Halt) {
		if ((FPlatformTime::Seconds() - LastHalt) >= HaltTime) {
			Halt = false;
		}
		return;
	}
	this->AddMovementInput(FVector{ 1,0,0 }, Direction * WalkVelocity);
}

void AEnemyOne::ReverseDirection() {
	Direction = Direction * -1;
}

void AEnemyOne::MakeHalt() {
	LastHalt = FPlatformTime::Seconds();
	Halt = true;
}