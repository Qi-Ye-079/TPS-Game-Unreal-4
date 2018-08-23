// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PowerupActor.generated.h"

UCLASS()
class TPSTUTORIAL_API APowerupActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APowerupActor();

protected:

	// Time between the powerup ticks in seconds
	UPROPERTY(EditDefaultsOnly, Category = "Properties")
	float PowerupInterval;

	// Total number of ticks to apply the powerup effect
	UPROPERTY(EditDefaultsOnly, Category = "Properties")
	int32 TotalNumOfTicks;

	// Total number of ticks applied. if exceeding the Total number of ticks, ticking should expire
	int32 TotalNumOfTicksProcessed;

	FTimerHandle TimerHandle_PowerupTicks;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void TickPowerup();
public:	

	// Activate power up when it's picked up
	void ActivatePowerup();

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerups")
	void OnActivated();

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerups")
	void OnPowerupTicked();
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Powerups")
	void OnExpired();
};
