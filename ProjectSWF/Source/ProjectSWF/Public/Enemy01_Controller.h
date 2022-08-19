// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Enemy01_Controller.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTSWF_API AEnemy01_Controller : public AAIController
{
	GENERATED_BODY()

public:
	// Called every frame
	void Tick(float DeltaTime) override;
	
	UFUNCTION(BlueprintCallable, Category = "Action")
		void WalkTowardsPlayer();

	UFUNCTION(BlueprintCallable, Category = "Player")
		bool ReturnPlayerSpotted();

private:
	bool PlayerSpotted = false;
};
