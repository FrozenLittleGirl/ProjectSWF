// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SStatusComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogStatusComponent, Log, All);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnHealthChanged, AActor*, InstigatorActor, USStatusComponent*, OwningComp, float, NewHealth, float, Delta);


class USHurtComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NEWPROJECT_API USStatusComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USStatusComponent();

	UFUNCTION(BlueprintCallable)
	bool ApplyHealthChange(AActor* Instigator, float Delta);

protected:
	UPROPERTY(BlueprintAssignable)
	FOnHealthChanged OnHealthChanged;
	
	UPROPERTY(EditAnywhere)
	bool ShowDebugDisplay;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
	float MaxHealth;

	UFUNCTION(BlueprintCallable)
	bool IsAlive() const;

	UFUNCTION(BlueprintCallable)
	float GetCurrentHealth();
	
	// Called when the game starts
	virtual void BeginPlay() override;
	
private:
	float CurrentHealth;
};
