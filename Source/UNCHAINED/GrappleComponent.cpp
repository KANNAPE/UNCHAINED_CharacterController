// Fill out your copyright notice in the Description page of Project Settings.


#include "GrappleComponent.h"
#include "GrappleHook.h"
#include "GrappleCable.h"
#include "CableComponent.h"
#include "UNCHAINEDCharacter.h"
#include "UnleashedMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Math/Vector.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values for this component's properties
UGrappleComponent::UGrappleComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UGrappleComponent::BeginPlay()
{
	Super::BeginPlay();	
}

void UGrappleComponent::OnGrappleHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
	const auto* Character = Cast<AUNCHAINEDCharacter>(GetOwner()); 
	if(IsValid(Character))    
	{
		auto* CharacterMovementComponent = Cast<UUnleashedMovementComponent>(Character->GetMovementComponent());
		if (IsValid(CharacterMovementComponent))
		{
			State = EGrappleState::Attached;
			
			CharacterMovementComponent->GroundFriction = 0.f;
			CharacterMovementComponent->GravityScale = 0.f;
			CharacterMovementComponent->AirControl = 0.2f;
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Friction :: %f"), CharacterMovementComponent->GroundFriction));

			CharacterMovementComponent->Velocity = ToGrappleHook() * GrappleForce;

			InitialHookDirection2D = ToGrappleHook2D();
		}
	}
}

void UGrappleComponent::OnGrappleDestroyed(AActor* DestroyedActor)
{
	UWorld* World = GetWorld();
	const auto* Character = Cast<AUNCHAINEDCharacter>(GetOwner());
	
	if (IsValid(World) && IsValid(Character))    
	{
		State = EGrappleState::Ready;
		
		if(IsValid(GrappleCable))
		{
			World->DestroyActor(GrappleCable);
			GrappleCable = nullptr;
		}

		auto* CharacterMovementComponent = Cast<UCharacterMovementComponent>(Character->GetMovementComponent());
		if (IsValid(CharacterMovementComponent))
		{
			CharacterMovementComponent->GroundFriction = 1.f;
			CharacterMovementComponent->GravityScale = 1.f;
			CharacterMovementComponent->AirControl = 0.05f;

			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("Friction :: %f"), CharacterMovementComponent->GroundFriction));
		}
	}
}

FVector UGrappleComponent::LocalToWorldOffset(const FVector& LocalOffset) const
{
	const AActor* Owner = GetOwner(); 
	if(IsValid(Owner))
	{
		const FVector Location = Owner->GetActorLocation();
		const FTransform Transform = Owner->GetTransform();

		return Location + UKismetMathLibrary::TransformDirection(Transform, LocalOffset);
	}

	return FVector::Zero();
}

FVector UGrappleComponent::ToGrappleHook() const
{
	const AActor* Owner = GetOwner(); 
	if(IsValid(Owner) && IsValid(GrappleHook))
	{
		const FVector HookLocation = GrappleHook->GetActorLocation();
		const FVector OwnerLocation = Owner->GetActorLocation();

		return UKismetMathLibrary::Normal(HookLocation - OwnerLocation);
	}

	return FVector::Zero();
}

FVector2D UGrappleComponent::ToGrappleHook2D() const
{
	return UKismetMathLibrary::Normal2D(FVector2D(ToGrappleHook()));
}

void UGrappleComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(State != EGrappleState::Attached) return;

	const auto* Character = Cast<AUNCHAINEDCharacter>(GetOwner()); 
	if(IsValid(Character))
	{
		// Add Force to the player
		auto* CharacterMovement = Cast<UCharacterMovementComponent>(Character->GetMovementComponent());
		if(IsValid(CharacterMovement))
		{
			CharacterMovement->AddForce(ToGrappleHook() * 100000.f);
		}		

		// Check whether we can destroy the grapple hook
		if(!IsValid(GrappleHook)) return;
		
		const FVector HookLocation = GrappleHook->GetActorLocation();
		const FVector CharacterLocation = Character->GetActorLocation();
		
		if(FVector::DistSquared(HookLocation, CharacterLocation) < 62500.f)
		{
			GetWorld()->DestroyActor(GrappleHook);
			return;
		}
		
		if(FVector2D::DotProduct(InitialHookDirection2D, ToGrappleHook2D()) >= 0.f) return;

		GetWorld()->DestroyActor(GrappleHook);
	}
}

void UGrappleComponent::FireGrapple(const FVector& TargetLocation, const FVector& LocalOffset)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Fire")));

	UWorld* World = GetWorld(); 
	if(IsValid(World))
	{
		State = EGrappleState::Firing;
		
		// Spawn Hook
		const FVector Offset = LocalToWorldOffset(LocalOffset);
		const FVector GrappleDirection = UKismetMathLibrary::Normal(TargetLocation - Offset);
		const FVector Velocity = GrappleDirection * Speed;

		GrappleHook = World->SpawnActorDeferred<AGrappleHook>(GrappleHookDefault, FTransform(Offset));
		
		
		if(!IsValid(GrappleHook)) return;
		GrappleHook->Velocity = Velocity;

		GrappleHook->FinishSpawning(FTransform(Offset));

		//	Bind events on Hook
		GrappleHook->OnActorHit.AddUniqueDynamic(this, &UGrappleComponent::OnGrappleHit);
		GrappleHook->OnDestroyed.AddUniqueDynamic(this, &UGrappleComponent::OnGrappleDestroyed);
	
		// Spawn Cable
		GrappleCable = World->SpawnActor<AGrappleCable>(GrappleCableDefault, Offset, UKismetMathLibrary::MakeRotFromX(GrappleDirection));
		AActor* Owner = GetOwner(); 
		if(IsValid(Owner) && IsValid(GrappleCable))
		{
			if (UCableComponent* HookCableComponent = Cast<UCableComponent>(
				GrappleCable->GetComponentByClass(UCableComponent::StaticClass())); IsValid(HookCableComponent))
			{
				GrappleCable->AttachToActor(Owner, FAttachmentTransformRules(EAttachmentRule::KeepWorld, true));
				HookCableComponent->SetAttachEndTo(GrappleHook, NAME_None, NAME_None);				
			}				
		}
	}
}

void UGrappleComponent::CancelGrapple() const
{
	if(IsValid(GrappleHook))
	{
		GetWorld()->DestroyActor(GrappleHook);
	}
}

