// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TpsWeapon.h"
#include "TpsCharacter.generated.h"

// Pre-declaration
class UCameraComponent;
class USpringArmComponent;

UCLASS()
class TPSTUTORIAL_API ATpsCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATpsCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Shoot weapon function; will be looping if the current weapon is automatic
	UFUNCTION(BlueprintCallable)
	void ShootWeapon();

	// dynamic delegate function on health changed
	UFUNCTION()
	void HandleHealthUpdate(class UTpsHealthComponent* OwningHealthComp, float CurrentHealth, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	// Bind axis inputs
	void MoveForward(float axisValue);
	void MoveRight(float axisValue);
	//BInd action
	void BeginCrouch();
	void EndCrouch();
	void StartShoot();
	void EndShoot();
	void ZoomIn();
	void ZoomOut();

	// Main camera component
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components")
	UCameraComponent* CameraComp;

	// Spring Arm component
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components")
	USpringArmComponent* SpringArmComp;

	// Health component
	UTpsHealthComponent* HealthComp;

	// The weapon class to spawn the weapon
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<ATpsWeapon> WeaponClass;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<UCameraShake> CamShakeClass;

	// The spawned weapon instance
	ATpsWeapon* CurrentWeapon;

	// The timer handler for shooting weapon
	FTimerHandle TimerHandle;

	// The last fire time
	float LastFireTime;

	// The camera's zoom properties
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera Zoom")
	bool ZoomingIn = false; // false by default

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera Zoom")
	float ZoomSpeed = 10.f; // 10.f by default

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera Zoom")
	float ZoomInFov = 45.f; // 45.f by default

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera Zoom")
	float DefaultFov = 90.f; // 90.f by default

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera Zoom")
	float ZoomAlpha = 0; // 0 by default

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera Zoom")
	float ZoomHeight = 75.f; // 75.f by default

	UPROPERTY(BlueprintReadOnly, Category = "Player")
	bool bDead = false;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
