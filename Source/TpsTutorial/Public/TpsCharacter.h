// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
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

	// Bind axis inputs
	void MoveForward(float axisValue);
	void MoveRight(float axisValue);
	//BInd action
	void BeginCrouch();
	void EndCrouch();
	void ShootWeapon();
	void ZoomIn();
	void ZoomOut();

	// Main camera component
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components")
	UCameraComponent* CameraComp;

	// Spring Arm component
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components")
	USpringArmComponent* SpringArmComp;

	// The weapon class to spawn the weapon
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<class ATpsWeapon> WeaponClass;

	// The spawned weapon instance
	ATpsWeapon* CurrentWeapon;

	// The camera's zoom properties
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera Zoom")
	bool ZoomingIn; // false by default

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera Zoom")
	float ZoomSpeed; // 10.f by default

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera Zoom")
	float ZoomInFov; // 45.f by default

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera Zoom")
	float DefaultFov; // 90.f by default

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera Zoom")
	float ZoomAlpha; // 0 by default

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera Zoom")
	float ZoomHeight; // 70.f by default


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
