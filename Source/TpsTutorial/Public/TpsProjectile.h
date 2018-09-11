// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TpsProjectile.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class USceneComponent;
class UProjectileMovementComponent;

UCLASS()
class TPSTUTORIAL_API ATpsProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATpsProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Handles the movement of this projectile
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UProjectileMovementComponent* ProjectileMoveComp;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Launch the projectile
	void Launch(float LaunchSpeed);
	
};
