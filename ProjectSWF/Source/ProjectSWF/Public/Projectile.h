// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

class UProjectileMovementComponent;

UCLASS()
class PROJECTSWF_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Values")
		int32 Damage = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Values")
		float PersistTime = 0.26;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Values")
		FVector Force = { 450, 0, 0 };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Values")
		float ActivateTime = 0.26;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void LaunchProjectile(FVector Angle);

private:
	UProjectileMovementComponent* MovementComponent = nullptr;

	UPROPERTY(EditAnywhere, Category = "Values")
		float LaunchSpeed = 0.26;

	UFUNCTION(BlueprintCallable, Category = "Values")
		int32 ReturnDirection();

	UFUNCTION(BlueprintCallable, Category = "Setup")
		void AttachMovement(UProjectileMovementComponent* Component);

};
