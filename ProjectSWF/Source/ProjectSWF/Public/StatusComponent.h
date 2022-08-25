// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StatusComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTSWF_API UStatusComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UStatusComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY(EditAnywhere, Category = "Setup")
		int32 Health = 5;

	bool Died = false;
	float DyingCount = 0;

	// Functions

public:
	// return current health
	UFUNCTION(BlueprintCallable, Category = "CurrentStatus")
		int32 ReturnHealth();

	// return true if the actor is died and false otherwise
	UFUNCTION(BlueprintCallable, Category = "CurrentStatus")
		bool DiedOrNot();

	// substract health by given damage
	void TakeDamage(int32 Damage);

	float ReturnDyingCount();
	
};
