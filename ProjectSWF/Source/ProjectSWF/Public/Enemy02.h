// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy02.generated.h"

class UStatusComponent;
class AHitBoxActor;

UCLASS()
class PROJECTSWF_API AEnemy02 : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy02();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category = "Damage")
		void TakeDamage(int32 Damage, int32 ForceDirection, FVector Force);

private:

	UPROPERTY(EditAnywhere, Category = "Setup")
		float WalkVelocity = 1;

	UPROPERTY(EditAnywhere, Category = "Setup")
		int32 Direction = 1;

	bool PlayerSpotted = false;

	UStatusComponent* Status = nullptr;

	// Halt

	UPROPERTY(EditAnywhere, Category = "Setup")
		float HaltTime = 2;

	float LastHalt = 0;

	bool Halt = false;

	// Attack Player
	UPROPERTY(EditAnywhere, Category = "Setup")
		float DistancePlayer = 140;

	UPROPERTY(EditAnywhere, Category = "Setup")
		TSubclassOf<AHitBoxActor> HitBoxBluePrint;

	// Generate the hit box
	UFUNCTION(BlueprintCallable, Category = "Hit")
		void SpawnHitBox(int32 Damage, float Time, FVector Location, FRotator Rotation, float CapsuleHight, float CapsuleRadius, int32 DirectionTo, FVector ForceTo);

protected:
	// General
	bool Attacking = false;

	//------- Basic Actions ---------
	UFUNCTION(BlueprintCallable, Category = "Player")
		bool ReturnPlayerSpotted();

	UFUNCTION(BlueprintCallable, Category = "Action")
		void ReverseDirection();

	UFUNCTION(BlueprintCallable, Category = "Action")
		void MakeHalt();

	UFUNCTION(BlueprintCallable, Category = "Action")
		void WalkTowardsPlayer();

	UFUNCTION(BlueprintCallable, Category = "Action")
		void LoseTrackOfPlayer();

	void Walk();

	UFUNCTION(BlueprintCallable, Category = "Attack")
		void BasicAttack(int32 DirectionTo);

	//------- Functions ---------
	// Attach Status from the bluepirnt
	UFUNCTION(BlueprintCallable, Category = "Setup")
		void AttachStatus(UStatusComponent* NewStatus);

	float LastAttack = 0;

	// Basic Attack Setup
	UPROPERTY(EditAnywhere, Category = "BasicAttack")
		float BasicAttackInterval = 4;

	UPROPERTY(EditAnywhere, Category = "BasicAttack")
		float BasicAttackActivateTime = 0.26;

	UPROPERTY(EditAnywhere, Category = "BasicAttack")
		float BasicAttackRadius = 39.1;

	UPROPERTY(EditAnywhere, Category = "BasicAttack")
		float BasicAttackHight = 84.6;

	UPROPERTY(EditAnywhere, Category = "BasicAttack")
		FVector BasicAttackSpawnLocation = { 56.69, 0, -11 };

	UPROPERTY(EditAnywhere, Category = "BasicAttack")
		FRotator BasicAttackSpawnRotation = { 0, -90, 0 };

	UPROPERTY(EditAnywhere, Category = "BasicAttack")
		FVector BasicAttackForce = { 450, 0, 0 };

	UFUNCTION(BlueprintCallable, Category = "BasicAttack")
		void SpawnBasicAttack(int32 DirectionTo);


};
