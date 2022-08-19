// Fill out your copyright notice in the Description page of Project Settings.

#include "../Public/StatusComponent.h"
#include "../ProjectSWFCharacter.h"
#include "Enemy02.h"

// Sets default values
AEnemy02::AEnemy02()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Status = CreateDefaultSubobject<UStatusComponent>(TEXT("Status"));
	Status->SetupVariables(false, Health, BasicDamage);
}

// Called when the game starts or when spawned
void AEnemy02::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemy02::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PlayerSpotted) {
		auto Player = Cast<AProjectSWFCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
		float interval = Player->GetTargetLocation().X - GetTargetLocation().X;
		if (interval < 0) {
			Direction = -1;
		}
		else {
			Direction = 1;
		}
		if (DistancePlayer < abs(interval)) {
			Walk();
		}
	}
	else {
		Walk();
	}
}

// Called to bind functionality to input
void AEnemy02::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy02::TakingDamage() {
	UE_LOG(LogTemp, Warning, TEXT("%s is taking damage"), *(GetName()));
}

void AEnemy02::Walk() {	
	// Enemy will not halt if player is spotted
	if (Halt) {
		if ((FPlatformTime::Seconds() - LastHalt) >= HaltTime) {
			Halt = false;
		}
		return;
	}

	this->AddMovementInput(FVector{ 1,0,0 }, Direction * WalkVelocity);
}

void AEnemy02::ReverseDirection() {
	Direction = Direction * -1;
}

void AEnemy02::MakeHalt() {
	LastHalt = FPlatformTime::Seconds();
	Halt = true;
}

bool AEnemy02::ReturnPlayerSpotted() {
	return PlayerSpotted;
}

void AEnemy02::WalkTowardsPlayer() {
	PlayerSpotted = true;
	UE_LOG(LogTemp, Warning, TEXT("%s: Player Spotted!"), *(GetName()));
}

void AEnemy02::LoseTrackOfPlayer() {
	UE_LOG(LogTemp, Warning, TEXT("%s: Lose Track of Player!"), *(GetName()));
	PlayerSpotted = false;
	MakeHalt();
}