// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "UNCHAINEDCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UAnimMontage;
class USoundBase;

// Declaration of the delegate that will be called when the Primary Action is triggered
// It is declared as dynamic so it can be accessed also in Blueprints
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUseItem);

UCLASS(config=Game)
class AUNCHAINEDCharacter : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	USkeletalMeshComponent* Mesh1P;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
	class UGrappleComponent* GrappleComponent;

public:
	AUNCHAINEDCharacter(const FObjectInitializer& ObjectInitializer);	

protected:
	// APawn interface implementation
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	
	// Unreal callbacks
	virtual void BeginPlay();

	// Action callbacks
	void OnPrimaryAction();
	void OnGrapplingAction();
	void OnDashingAction();
	void OnSlidingAction();

	// Axis callbacks
	void MoveForward(float Val);
	void MoveRight(float Val);
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);

public:
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera) float TurnRateGamepad;
	UPROPERTY(BlueprintAssignable, Category = "Interaction") FOnUseItem OnUseItem;
};

