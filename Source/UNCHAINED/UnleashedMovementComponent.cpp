// Fill out your copyright notice in the Description page of Project Settings.


#include "UnleashedMovementComponent.h"

#include "UNCHAINEDCharacter.h"

void UUnleashedMovementComponent::InitializeComponent()
{
	Super::InitializeComponent();
	Character = Cast<AUNCHAINEDCharacter>(GetOwner());
}

void UUnleashedMovementComponent::PhysCustom(float DeltaTime, int32 Iterations)
{
	Super::PhysCustom(DeltaTime, Iterations);

	switch(static_cast<ECustomMovementMode>(CustomMovementMode))
	{
	case ECustomMovementMode::CMOVE_Dashing:
		PhysDashing(DeltaTime, Iterations);
		
	case ECustomMovementMode::CMOVE_Sliding:
		PhysSliding(DeltaTime, Iterations);
		
	case ECustomMovementMode::CMOVE_WallRiding:
		PhysWallRiding(DeltaTime, Iterations);
		
	case ECustomMovementMode::CMOVE_None:
	default:
		break;
	}
}

void UUnleashedMovementComponent::PhysDashing(float DeltaTime, int32 Iterations)
{
	if (DeltaTime < MIN_TICK_TIME)
	{
		return;
	}
	
	DashTimer += DeltaTime;

	if(DashTimer >= .1f)
	{
		SetMovementMode(EMovementMode::MOVE_Falling);
	}
}

void UUnleashedMovementComponent::PhysSliding(float DeltaTime, int32 Iterations)
{
	
}

void UUnleashedMovementComponent::PhysWallRiding(float DeltaTime, int32 Iterations)
{
	
}

void UUnleashedMovementComponent::OnMovementModeChanged(const EMovementMode PreviousMovementMode, const uint8 PreviousCustomMode = 0)
{
	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);
}

bool UUnleashedMovementComponent::CanDash() const
{
	return MovementMode != EMovementMode::MOVE_Custom || CustomMovementMode != static_cast<uint8>(ECustomMovementMode::CMOVE_Dashing);
}

void UUnleashedMovementComponent::PerformDash()
{
	if(CanDash())
	{
		DashTimer = 0.f;
		SetMovementMode(MOVE_Custom, static_cast<uint8>(ECustomMovementMode::CMOVE_Dashing));
		PreDashHorizontalVelocity = FVector(Velocity.X, Velocity.Y, 0.);
	}
}
