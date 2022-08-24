// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HitBoxActor.generated.h"

class UCapsuleComponent;

UCLASS()
class PROJECTSWF_API AHitBoxActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHitBoxActor();

	UFUNCTION(BlueprintCallable, Category = "Return")
		int32 ReturnDamage();

	UFUNCTION(BlueprintCallable, Category = "Return")
		float ReturnPersistTime();

	void StoreValues(int32 DamageAmount, float Time);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void InitializeHitBox(FVector Location, FRotator Rotation, float Hight, float Radius);

	void InitializeValues(int32 DirectionTo, FVector ForceTo);

private:
	UCapsuleComponent* HitBox = nullptr;

	int32 Damage = 1;
	float PersistTime = 0.26;

	int32 Direction = 1;
	FVector Force = { 450, 0, 0 };

	UFUNCTION(BlueprintCallable, Category = "Setup")
		void AttachHitBox(UCapsuleComponent* Box);

};
