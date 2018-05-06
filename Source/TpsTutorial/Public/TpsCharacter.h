// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TpsCharacter.generated.h"

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

	// Input functions for axis
	void MoveForward(float axisValue);
	void MoveRight(float axisValue);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	
	
};
