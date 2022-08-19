// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy02.generated.h"

class UStatusComponent;

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

	UFUNCTION(BlueprintCallable, Category = "Player")
		bool ReturnPlayerSpotted();

	void Walk();

private:
	UPROPERTY(EditAnywhere, Category = Setup)
		int32 Health = 5;

	UPROPERTY(EditAnywhere, Category = Setup)
		int32 BasicDamage = 1;

	UPROPERTY(EditAnywhere, Category = Setup)
		float WalkVelocity = 1;

	UFUNCTION(BlueprintCallable, Category = "Damage")
		void TakingDamage();

	int Direction = 1;

	bool PlayerSpotted = false;

	UStatusComponent* Status;

	UFUNCTION(BlueprintCallable, Category = "Action")
		void ReverseDirection();

	// Halt

	UPROPERTY(EditAnywhere, Category = "Setup")
		float HaltTime = 2;

	float LastHalt = 0;

	bool Halt = false;

	UFUNCTION(BlueprintCallable, Category = "Action")
		void MakeHalt();

	// Attack Player
	UPROPERTY(EditAnywhere, Category = "Setup")
		float DistancePlayer = 140;

	UFUNCTION(BlueprintCallable, Category = "Action")
		void WalkTowardsPlayer();

	UFUNCTION(BlueprintCallable, Category = "Action")
		void LoseTrackOfPlayer();
};
