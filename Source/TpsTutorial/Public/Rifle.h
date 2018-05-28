// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TpsWeapon.h"
#include "Rifle.generated.h"

/**
 * 
 */
UCLASS()
class TPSTUTORIAL_API ARifle : public ATpsWeapon
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	ARifle();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// The fire function of this weapon
	UFUNCTION(BlueprintCallable)
	virtual void Fire(const bool& IsHit, const FHitResult& HitRes, const FVector& TracingEndLocation) override;

	// Character can have access to all the members above.
	friend class ATpsCharacter;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	
};
