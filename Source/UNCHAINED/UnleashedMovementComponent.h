// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UnleashedMovementComponent.generated.h"

UENUM(BlueprintType)
enum class ECustomMovementMode : uint8
{
	CMOVE_None			UMETA(Hidden),
	CMOVE_Dashing		UMETA(DisplayName = "Dashing"),
	CMOVE_Sliding		UMETA(DisplayName = "Sliding"),
	CMOVE_WallRiding	UMETA(DisplayName = "Wall Riding"),
	CMOVE_MAX			UMETA(Hidden)
};

/**
 * 
 */
UCLASS()
class UNCHAINED_API UUnleashedMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

	// Dash
	float DashTimer = 0.f;
public:
	FVector PreDashHorizontalVelocity;

protected:
	virtual void InitializeComponent() override;

	// Phys methods
	virtual void PhysCustom(float DeltaTime, int32 Iterations) override;
	virtual void PhysDashing(float DeltaTime, int32 Iterations);
	virtual void PhysSliding(float DeltaTime, int32 Iterations);
	virtual void PhysWallRiding(float DeltaTime, int32 Iterations);

	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;
	//virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;

	bool CanDash() const;
	bool CanSlide() const;
	bool CanWallRide() const;

	UPROPERTY()
	class AUNCHAINEDCharacter* Character;

public:
	void PerformDash();
	void PerformSlide();
	void PerformWallRide();
};
