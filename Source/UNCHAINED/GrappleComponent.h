// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GrappleComponent.generated.h"

UENUM(BlueprintType)
enum class EGrappleState : uint8
{
	Ready,
	Firing,
	Attached
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UNCHAINED_API UGrappleComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGrappleComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// Callbacks
	UFUNCTION()
	void OnGrappleHit(class AActor* SelfActor, class AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);
	UFUNCTION()
	void OnGrappleDestroyed(class AActor* DestroyedActor);

	// Macros
	FVector LocalToWorldOffset(const FVector& LocalOffset) const;
	FVector ToGrappleHook() const;
	FVector2D ToGrappleHook2D() const;

	// Properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Constants")
	float Speed = 5000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGrappleState State = EGrappleState::Ready;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grappling Hook")
	TSubclassOf<class AGrappleHook> GrappleHookDefault;
	UPROPERTY()
	class AGrappleHook* GrappleHook;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grappling Hook")
	TSubclassOf<class AGrappleCable> GrappleCableDefault;
	UPROPERTY()
	class AGrappleCable* GrappleCable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float GrappleForce = 1200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D InitialHookDirection2D;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void FireGrapple(const FVector& TargetLocation, const FVector& LocalOffset);
	void CancelGrapple() const;
	
	FORCEINLINE bool HasBeenLaunched() const { return State == EGrappleState::Firing || State == EGrappleState::Attached; }
};
