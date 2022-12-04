// Copyright Epic Games, Inc. All Rights Reserved.

#include "UNCHAINEDCharacter.h"

#include "GrappleComponent.h"
#include "UNCHAINEDProjectile.h"
#include "UnleashedMovementComponent.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"


//////////////////////////////////////////////////////////////////////////
// AUNCHAINEDCharacter

AUNCHAINEDCharacter::AUNCHAINEDCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UUnleashedMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	TurnRateGamepad = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	// Grapple Component
	GrappleComponent = CreateDefaultSubobject<UGrappleComponent>(TEXT("GrappleComponent"));

}

void AUNCHAINEDCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
}

//////////////////////////////////////////////////////////////////////////// Input

void AUNCHAINEDCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("PrimaryAction", IE_Pressed, this, &AUNCHAINEDCharacter::OnPrimaryAction);

	// Bind movement events
	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &AUNCHAINEDCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &AUNCHAINEDCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "Mouse" versions handle devices that provide an absolute delta, such as a mouse.
	// "Gamepad" versions are for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn Right / Left Gamepad", this, &AUNCHAINEDCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("Look Up / Down Gamepad", this, &AUNCHAINEDCharacter::LookUpAtRate);

	// Unleashed actions
	PlayerInputComponent->BindAction("Grapple", IE_Pressed, this, &AUNCHAINEDCharacter::OnGrapplingAction);
	//PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &AUNCHAINEDCharacter::OnDashingAction);
	//PlayerInputComponent->BindAction("Slide", IE_Pressed, this, &AUNCHAINEDCharacter::OnSlidingAction);
}

void AUNCHAINEDCharacter::OnPrimaryAction()
{
	// Trigger the OnItemUsed Event
	OnUseItem.Broadcast();
}

// To be converted in a custom movement mode
void AUNCHAINEDCharacter::OnGrapplingAction()
{
	if (!IsValid(GrappleComponent)) return;

	if (GrappleComponent->HasBeenLaunched())
	{
		GrappleComponent->CancelGrapple();
		return;
	}

	if (!IsValid(FirstPersonCameraComponent)) return;

	const FVector StartLocation = FirstPersonCameraComponent->GetComponentTransform().GetLocation();
	const FVector EndLocation = StartLocation + FirstPersonCameraComponent->GetForwardVector() * 25000.f;
	const FVector LocalOffset(-30.f, -30.f, 50.f);
	FHitResult Hit;

	if (GetWorld()->LineTraceSingleByChannel(Hit, StartLocation, EndLocation, ECollisionChannel::ECC_Visibility))
	{
		GrappleComponent->FireGrapple(Hit.Location, LocalOffset);
		return;
	}

	GrappleComponent->FireGrapple(Hit.TraceEnd, LocalOffset);
}

// Still to be implemented
void AUNCHAINEDCharacter::OnDashingAction()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("Woosh!")));
}

// Still to be implemented
void AUNCHAINEDCharacter::OnSlidingAction()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("Shoosh!")));
}


void AUNCHAINEDCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}
void AUNCHAINEDCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}
void AUNCHAINEDCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}
void AUNCHAINEDCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}