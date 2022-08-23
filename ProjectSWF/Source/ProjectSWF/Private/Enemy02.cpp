// Fill out your copyright notice in the Description page of Project Settings.

#include "../Public/StatusComponent.h"
#include "../ProjectSWFCharacter.h"
#include "../Public/HitBoxActor.h"
#include "Enemy02.h"

// Sets default values
AEnemy02::AEnemy02()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//this->MoveIgnoreActorAdd(this);
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

		// Try to attack or approach player
		if (DistancePlayer < abs(interval)) {
			Walk();
		} 
		else {
			// see if enemy is attacking or is trying to attack
			if (Attacking) {
				if ((FPlatformTime::Seconds() - LastAttack) >= BasicAttackInterval) {
					Attacking = false;
				}
			}
			else {
				BasicAttack();
			}
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

void AEnemy02::TakeDamage(int32 Damage) {
	UE_LOG(LogTemp, Warning, TEXT("%s is taking damage: %d"), *(GetName()), Damage);
	Status->TakeDamage(Damage);
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
	if (Direction >= 0) {
		this->SetActorRotation(FRotator(0, 0, 0));
	}
	else {
		this->SetActorRotation(FRotator(0, 180, 0));
	}
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
	Halt = false;
	UE_LOG(LogTemp, Warning, TEXT("%s: Player Spotted!"), *(GetName()));
}

void AEnemy02::LoseTrackOfPlayer() {
	UE_LOG(LogTemp, Warning, TEXT("%s: Lose Track of Player!"), *(GetName()));
	PlayerSpotted = false;
	MakeHalt();
}

void AEnemy02::AttachStatus(UStatusComponent* NewStatus) {
	Status = NewStatus;
}

void AEnemy02::SpawnHitBox(int32 Damage, float Time, FVector Location, FRotator Rotation, float CapsuleHight, float CapsuleRadius) {
	FRotator ActorRotation = FRotator{ 0,0,0 };
	if (GetActorRotation().Yaw != 0) {
		ActorRotation = FRotator{ 0,180,0 };
	}
	auto HitBox = GetWorld()->SpawnActor<AHitBoxActor>(
		HitBoxBluePrint,
		GetTargetLocation(),
		ActorRotation
		);

	HitBox->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
	HitBox->StoreValues(Damage, Time);
	HitBox->InitializeHitBox(Location, Rotation, CapsuleHight, CapsuleRadius);
}

void AEnemy02::SpawnBasicAttack() {
	SpawnHitBox(Status->ReturnBasicDamage(), BasicAttackActivateTime, BasicAttackSpawnLocation,
		BasicAttackSpawnRotation, BasicAttackHight, BasicAttackRadius);
}

void AEnemy02::BasicAttack() {
	if (Attacking) {
		return;
	} else {
		Attacking = true;
		LastAttack = FPlatformTime::Seconds();
	}

	SpawnBasicAttack();
}