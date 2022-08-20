// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "EnemyOne.generated.h"

class UStatusComponent;
class UCapsuleComponent;

UCLASS()
class PROJECTSWF_API AEnemyOne : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AEnemyOne();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

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
	
	UStatusComponent* Status = nullptr;

	UFUNCTION(BlueprintCallable, Category = "Player")
		bool ReturnPlayerSpotted();

	UFUNCTION(BlueprintCallable, Category = "Action")
		void ReverseDirection();

	// Halt

	UPROPERTY(EditAnywhere, Category = "Setup")
		float HaltTime = 2;

	float LastHalt = 0;

	bool Halt = false;

	UFUNCTION(BlueprintCallable, Category = "Action")
		void MakeHalt();
};
