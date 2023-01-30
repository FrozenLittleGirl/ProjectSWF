// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PaperCharacter.h"
#include "ProjectSWFCharacter.generated.h"

class UTextRenderComponent;
class UStatusComponent;
class AHitBoxActor;
class UPaperFlipbook;

UENUM(BlueprintType)
enum class EState : uint8 {
	EState_Idle				UMETA(DisplayName="Idle"),
	EState_BasicAttack		UMETA(DisplayName="BasicAttack"),
	EState_Dodge			UMETA(DisplayName="Dodge"),
	EState_Jump				UMETA(DisplayName="Jump"),
	EState_Fall				UMETA(DisplayName="Fall"),
	EState_Run				UMETA(DisplayName="Run"),
	EState_Die				UMETA(DisplayName="Die"),
	EState_DieLoop			UMETA(DisplayName="DieLoop"),
};

/**
 * This class is the default character for ProjectSWF, and it is responsible for all
 * physical interaction between the player and the world.
 *
 * The capsule component (inherited from ACharacter) handles collision with the world
 * The CharacterMovementComponent (inherited from ACharacter) handles movement of the collision capsule
 * The Sprite component (inherited from APaperCharacter) handles the visuals
 */
UCLASS(config=Game)
class AProjectSWFCharacter : public APaperCharacter
{
	GENERATED_BODY()

	/** Side view camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera, meta=(AllowPrivateAccess="true"))
	class UCameraComponent* SideViewCameraComponent;

	/** Camera boom positioning the camera beside the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UTextRenderComponent* TextComponent;
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

	/** Called to choose the correct animation to play based on the character's movement state */
	void UpdateAnimation();

	/** Called for side to side input */
	void MoveRight(float Value);

	// Respawn Player
	void Revive();

	// This also update collision box position
	void UpdateCharacter();

	/** Handle touch inputs. */
	void TouchStarted(const ETouchIndex::Type FingerIndex, const FVector Location);

	/** Handle touch stop event. */
	void TouchStopped(const ETouchIndex::Type FingerIndex, const FVector Location);

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End of APawn interface

public:
	AProjectSWFCharacter();

	/** Returns SideViewCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetSideViewCameraComponent() const { return SideViewCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	UFUNCTION(BlueprintCallable, Category = "Damage")
		void TakeDamageNoDirection(int32 Damage);

	UFUNCTION(BlueprintCallable, Category = "Damage")
		void TakeDamage(int32 Damage, int32 ForceDirection, FVector Force);

	UFUNCTION(BlueprintCallable, Category = "Player")
		void ResetLastLevel(FName Level);

	FVector ReturnPlayerForce();

	bool DiedOrNot();

protected:
	UPROPERTY(BlueprintReadWrite)
	EState PlayerEState;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setup")
	float PlayDeathTime = 0.34;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Setup")
	FName LastLevel = "DevTest_Level";
	
	UPROPERTY(BlueprintReadWrite)
	UStatusComponent* StatusComp;

	UPROPERTY(EditDefaultsOnly, Category = "Dodge")
	float DodgeDelay;
	
	UPROPERTY(EditDefaultsOnly, Category = "Dodge")
	float DodgeHalt;
	
	UPROPERTY(EditDefaultsOnly, Category = "Dodge")
	FVector DodgeLaunchVelocity;
	
	FTimerHandle TimerHandle_Dodge;
	FTimerHandle TimerHandle_DodgeHalt;
	
	void Dodge();
	void Dodge_TimeElapsed();
	void DodgeHalt_TimeElapsed();
	
	// Attacking Actions
	UPROPERTY(EditDefaultsOnly)
	float BasicAttackDelay;
	
	FTimerHandle TimerHandle_BasicAttack;
	
	void BasicAttack();
	void BasicAttack_TimeElapsed();
	void BasicAttackAbort();

private:
	//------- Variables ---------
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		bool Attacking = false;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		bool Dodging = false;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		int32 NumDodge = 1;

	UPROPERTY(EditAnywhere, Category = "Setup")
		TSubclassOf<AHitBoxActor> HitBoxBluePrint;

	UPROPERTY(EditAnywhere, Category = "Setup")
		int32 MaxNumDodge = 1;

	float CountSeconds = 0;

	bool Dying = false;
	bool TotallyDied = false;

	UPROPERTY(EditAnywhere, Category = "Setup")
	FVector PlayerForce = { 10, 0, 0 };

	//------- Functions ---------
	// Generate the hit box
	UFUNCTION(BlueprintCallable, Category = "Hit")
	void SpawnHitBox(TSubclassOf<AHitBoxActor> Blueprint);

	// return face direction
	UFUNCTION(BlueprintCallable, Category = "Attack")
	int32 AttackDirection();

	//------- Basic Attack Setup ---------
	UFUNCTION(BlueprintCallable, Category = "BasicAttack")
	void SpawnBasicAttack();

	float GroundFriction;
	float Gravity;
	float JumpZVelocity;
	bool bDodgeHalt;
};
