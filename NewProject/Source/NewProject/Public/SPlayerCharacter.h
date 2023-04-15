// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperCharacter.h"
#include "SHurtComponent.h"
#include "SPlayerCharacter.generated.h"

class USHitComponent;
class USHurtComponent;
class UCapsuleComponent;
class UPaperFlipbook;
class USHitComponentManager;
class USStatusComponent;

UENUM(BlueprintType)
enum class ESpecialActionState : uint8 {
	BasicAttack		UMETA(DisplayName="BasicAttack"),
	Dodge			UMETA(DisplayName="Dodge"),
	Die				UMETA(DisplayName="Die"),
	DieLoop			UMETA(DisplayName="DieLoop"),
	None			UMETA(DisplayName="None")
};

/**
 * 
 */
UCLASS()
class NEWPROJECT_API ASPlayerCharacter : public APaperCharacter
{
	GENERATED_BODY()

	/** Side view camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera, meta=(AllowPrivateAccess="true"))
	class UCameraComponent* SideViewCameraComponent;

	/** Camera boom positioning the camera beside the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;
	
	virtual void Tick(float DeltaSeconds) override;

protected:
	// The animation to play while running around
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Animations)
	UPaperFlipbook* RunningAnimation;

	// The animation to play while idle (standing still)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	UPaperFlipbook* IdleAnimation;

	// ... To falling
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	UPaperFlipbook* StopJumpAnimation;
	
	// ... To Jump
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	UPaperFlipbook* WhileJumpAnimation;
	
	// ... To attack
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	UPaperFlipbook* BasicAttackAnimation;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	UPaperFlipbook* DodgeAnimation;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	UPaperFlipbook* DiedAnimation;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	UPaperFlipbook* DiedLoopAnimation;
	

	// ----------------------------------------------------------------------------------------
	UPROPERTY(BlueprintReadWrite)
	ESpecialActionState SpecialActionState;

	UPROPERTY(EditDefaultsOnly, Category = "Dodge")
	float DodgeDelay;
	
	UPROPERTY(EditDefaultsOnly, Category = "Dodge")
	float DodgeHalt;
	
	UPROPERTY(EditDefaultsOnly, Category = "Dodge")
	FVector DodgeLaunchVelocity;
	
	// Attacking Actions
	UPROPERTY(EditDefaultsOnly ,Category = "BasicAttack")
	float BasicAttackDelay;
	
	UPROPERTY(EditAnywhere ,Category = "BasicAttack")
	USHitComponent * BasicAttackHitBox;

	UPROPERTY(EditAnywhere ,Category = "HurtBax")
	USHurtComponent * HurtBox;

	UPROPERTY(EditAnywhere, Category = "Component")
	USHitComponentManager * HitComponentManager;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	USStatusComponent * StatusComponent;
	
	// return face direction
	UFUNCTION(BlueprintCallable, Category = "Attack")
	int AttackDirection();
	
	void Dodge();

	void BasicAttack();

	virtual void Jump() override;

	virtual void StopJumping() override;

private:
	FTimerHandle TimerHandle_Dodge;
	FTimerHandle TimerHandle_DodgeHalt;
	FTimerHandle TimerHandle_BasicAttack;

	int NumDodge;
	int MaxNumDodge;

	float GroundFriction;
	float Gravity;
	float JumpZVelocity;
	bool bDodgeHalt;
	
	void Dodge_TimeElapsed();
	void DodgeHalt_TimeElapsed();

	void BasicAttack_TimeElapsed();
	void BasicAttackAbort();

protected:
	/** Called to choose the correct animation to play based on the character's movement state */
	void UpdateAnimation();

	/** Called for side to side input */
	void MoveRight(float Value);

	void UpdateCharacter();

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End of APawn interface
	
public:
	ASPlayerCharacter();

	/** Returns SideViewCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetSideViewCameraComponent() const { return SideViewCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
};
