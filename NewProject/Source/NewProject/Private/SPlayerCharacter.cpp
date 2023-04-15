// Fill out your copyright notice in the Description page of Project Settings.

#include "NewProject/Public/SPlayerCharacter.h"

#include "PaperFlipbookComponent.h"
#include "SHitComponent.h"
#include "SHurtComponent.h"
#include "SHitComponentManager.h"
#include "SStatusComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

DEFINE_LOG_CATEGORY_STATIC(SideScrollerCharacter, Log, All);

ASPlayerCharacter::ASPlayerCharacter()
{
	// Use only Yaw from the controller and ignore the rest of the rotation.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// Create a camera boom attached to the root (capsule)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 500.0f;
	CameraBoom->SocketOffset = FVector(0.0f, 0.0f, 75.0f);
	CameraBoom->SetUsingAbsoluteRotation(true);
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	

	// Create an orthographic camera (no perspective) and attach it to the boom
	SideViewCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("SideViewCamera"));
	SideViewCameraComponent->ProjectionMode = ECameraProjectionMode::Orthographic;
	SideViewCameraComponent->OrthoWidth = 2048.0f;
	SideViewCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	// Prevent all automatic rotation behavior on the camera, character, and camera component
	CameraBoom->SetUsingAbsoluteRotation(true);
	SideViewCameraComponent->bUsePawnControlRotation = false;
	SideViewCameraComponent->bAutoActivate = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;

	// Configure character movement
	GetCharacterMovement()->GravityScale = 2.0f;
	GetCharacterMovement()->AirControl = 0.80f;
	GetCharacterMovement()->JumpZVelocity = 1000.f;
	GetCharacterMovement()->GroundFriction = 3.0f;
	GetCharacterMovement()->MaxWalkSpeed = 600.0f;
	GetCharacterMovement()->MaxFlySpeed = 600.0f;

	// Lock character motion onto the XZ plane, so the character can't move in or out of the screen
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->SetPlaneConstraintNormal(FVector(0.0f, -1.0f, 0.0f));

	// Behave like a traditional 2D platformer character, with a flat bottom instead of a curved capsule bottom
	// Note: This can cause a little floating when going up inclines; you can choose the tradeoff between better
	// behavior on the edge of a ledge versus inclines by setting this to true or false
	GetCharacterMovement()->bUseFlatBaseForFloorChecks = true;

	// Custom Setups
	BasicAttackHitBox = CreateDefaultSubobject<USHitComponent>("BasicAttackHitBox");
	BasicAttackHitBox->SetupAttachment(RootComponent);
	BasicAttackHitBox->DisableCollision();

	HurtBox = CreateDefaultSubobject<USHurtComponent>("HurtBox");
	HurtBox->SetupAttachment(RootComponent);
	HurtBox->SetCollisionProfileName(FName(TEXT("OverlapAll")));

	HitComponentManager = CreateDefaultSubobject<USHitComponentManager>("HitComponentManager");
	StatusComponent = CreateDefaultSubobject<USStatusComponent>("StatusComponent");
	
	SpecialActionState = ESpecialActionState::None;
	DodgeLaunchVelocity = FVector(2000.0f, 0.0f, 0.0f);
	DodgeDelay = 0.5f;
	DodgeHalt = 0.1f;
	BasicAttackDelay = 0.33f;
	bDodgeHalt = false;

	NumDodge = 1;
	MaxNumDodge = 1;
}

//////////////////////////////////////////////////////////////////////////
// Animation

void ASPlayerCharacter::UpdateAnimation()
{
	const FVector PlayerVelocity = GetVelocity();
	const float PlayerSpeedSqr = PlayerVelocity.SizeSquared();
	const float PlayerVelocityZ = PlayerVelocity.Z;

	UPaperFlipbook* DesiredAnimation = GetSprite()->GetFlipbook();
	
	switch (SpecialActionState)
	{
	case ESpecialActionState::Dodge:
		DesiredAnimation = DodgeAnimation;
		break;
	case ESpecialActionState::BasicAttack:
		DesiredAnimation = BasicAttackAnimation;
		break;
	case ESpecialActionState::None:
		if (PlayerVelocityZ > 0)
		{
			DesiredAnimation = WhileJumpAnimation;
		} else if (PlayerVelocityZ < 0)
		{
			DesiredAnimation = StopJumpAnimation;
		} else
		{
			DesiredAnimation = (PlayerSpeedSqr > 0.0f) ? RunningAnimation : IdleAnimation;
		}
		break;
	}

	if (GetSprite()->GetFlipbook() != DesiredAnimation)
	{
		GetSprite()->SetFlipbook(DesiredAnimation);
	}
}

void ASPlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// other
	if (NumDodge <= 0) {
		if (GetCharacterMovement()->IsFalling() == false) {
			NumDodge = MaxNumDodge;
		}
	}
	
	UpdateCharacter();	
}


//////////////////////////////////////////////////////////////////////////
// Input

void ASPlayerCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Note: the 'Jump' action and the 'MoveRight' axis are bound to actual keys/buttons/sticks in DefaultInput.ini (editable from Project Settings..Input)
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASPlayerCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ASPlayerCharacter::StopJumping);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASPlayerCharacter::MoveRight);

	PlayerInputComponent->BindAction("Dodge", IE_Pressed, this, &ASPlayerCharacter::Dodge);
	PlayerInputComponent->BindAction("BasicAttack", IE_Pressed, this, &ASPlayerCharacter::BasicAttack);
}

void ASPlayerCharacter::MoveRight(float Value)
{
	/*UpdateChar();*/

	// Apply the input to the character motion
	AddMovementInput(FVector(1.0f, 0.0f, 0.0f), Value);
}

void ASPlayerCharacter::UpdateCharacter()
{
	// Update animation to match the motion
	UpdateAnimation();

	// Now setup the rotation of the controller based on the direction we are travelling
	const FVector PlayerVelocity = GetVelocity();	
	float TravelDirection = PlayerVelocity.X;
	// Set the rotation so that the character faces his direction of travel.
	if (SpecialActionState != ESpecialActionState::None)
	{
		return;
	}
	if (Controller != nullptr)
	{
		if (TravelDirection < 0.0f)
		{
			Controller->SetControlRotation(FRotator(0.0, 180.0f, 0.0f));
		}
		else if (TravelDirection > 0.0f)
		{
			Controller->SetControlRotation(FRotator(0.0f, 0.0f, 0.0f));
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Other Setup

int ASPlayerCharacter::AttackDirection()
{
	int Direction;
	int XDirection = GetCharacterMovement()->GetLastUpdateVelocity().X;
	if (XDirection < 0) {
		Direction = -1;
	}
	else if (XDirection > 0) {
		Direction = 1;
	}
	else if (GetControlRotation().Yaw > 90) {
		Direction = -1;
	}
	else {
		Direction = 1;
	}
	return Direction;
}

void ASPlayerCharacter::Dodge()
{
	if (SpecialActionState == ESpecialActionState::Dodge || bDodgeHalt)
	{
		return;
	}
	if (GetMovementComponent()->IsFalling())
	{
		if (NumDodge <= 0)
		{
			return;
		} else
		{
			NumDodge -= 1;
		}
	}
	BasicAttackAbort();
	SpecialActionState = ESpecialActionState::Dodge;
	
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	DisableInput(PlayerController);
	
	JumpZVelocity = GetCharacterMovement()->JumpZVelocity;
	GroundFriction = GetCharacterMovement()->GroundFriction;
	Gravity = GetCharacterMovement()->GravityScale;
	GetCharacterMovement()->JumpZVelocity = 0.f;
	GetCharacterMovement()->GravityScale = 0.f;
	GetCharacterMovement()->GroundFriction = 0.f;
	
	LaunchCharacter(DodgeLaunchVelocity * AttackDirection(), true, true);
	GetWorldTimerManager().SetTimer(TimerHandle_Dodge, this, &ASPlayerCharacter::Dodge_TimeElapsed, DodgeDelay);
}

void ASPlayerCharacter::Jump()
{
	Super::Jump();
}

void ASPlayerCharacter::StopJumping()
{
	Super::StopJumping();
}

void ASPlayerCharacter::Dodge_TimeElapsed()
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	EnableInput(PlayerController);
	SpecialActionState = ESpecialActionState::None;
	GetCharacterMovement()->JumpZVelocity = JumpZVelocity;
	GetCharacterMovement()->GravityScale = Gravity;
	GetCharacterMovement()->GroundFriction = GroundFriction;
	bDodgeHalt = true;
	GetWorldTimerManager().SetTimer(TimerHandle_DodgeHalt, this, &ASPlayerCharacter::DodgeHalt_TimeElapsed, DodgeHalt);
}

void ASPlayerCharacter::DodgeHalt_TimeElapsed()
{
	bDodgeHalt = false;
}

void ASPlayerCharacter::BasicAttack()
{
	if (SpecialActionState != ESpecialActionState::None)
	{
		return;
	}
	SpecialActionState = ESpecialActionState::BasicAttack;
	BasicAttackHitBox->EnableCollision();
	GetWorldTimerManager().SetTimer(TimerHandle_BasicAttack, this, &ASPlayerCharacter::BasicAttack_TimeElapsed, BasicAttackDelay);
}

void ASPlayerCharacter::BasicAttack_TimeElapsed()
{
	SpecialActionState = ESpecialActionState::None;
	BasicAttackHitBox->DisableCollision();
}

void ASPlayerCharacter::BasicAttackAbort()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_BasicAttack);
}



