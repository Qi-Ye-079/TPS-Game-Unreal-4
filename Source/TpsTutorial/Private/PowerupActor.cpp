// Fill out your copyright notice in the Description page of Project Settings.

#include "PowerupActor.h"


// Sets default values
APowerupActor::APowerupActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void APowerupActor::BeginPlay()
{
	Super::BeginPlay();

}


void APowerupActor::ActivatePowerup()
{
	// Set a timer to tick between specified interval
	if (PowerupInterval > 0.f)
		GetWorldTimerManager().SetTimer(TimerHandle_PowerupTicks, this, &APowerupActor::TickPowerup, PowerupInterval, true, 0.f);
	else
		TickPowerup();
}


void APowerupActor::TickPowerup()
{
	// Keep track of total number of ticks applied
	TotalNumOfTicksProcessed++;

	// Process the power up tick
	OnPowerupTicked();

	// if reached total number of ticks: expire
	if (TotalNumOfTicksProcessed >= TotalNumOfTicks)
	{
		OnExpired();

		// Clear timer
		GetWorldTimerManager().ClearTimer(TimerHandle_PowerupTicks);
	}
}

