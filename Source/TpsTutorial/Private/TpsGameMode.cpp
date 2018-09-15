// Fill out your copyright notice in the Description page of Project Settings.

#include "TpsGameMode.h"
#include "TimerManager.h"
#include "Components/TpsHealthComponent.h"


ATpsGameMode::ATpsGameMode()
	:TimeBetweenWaves(2.f)
{
	// Make this mode class tick every 1 second
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.f;
}


void ATpsGameMode::StartPlay()
{
	Super::StartPlay();

	// Prepare for a wave
	PrepareForNextWave();
}


void ATpsGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Check wave status
	CheckWaveState();
}


void ATpsGameMode::StartWave()
{
	// Increment wave count at beginning of each wave and set the number of bots to spawn
	NumOfBotsToSpawn = 2 * (++WaveCount);

	// Set the timer to spawn bots periodically
	GetWorldTimerManager().SetTimer(TimerHandle_SpawnBots, this, &ATpsGameMode::SpawnBotTimerElapsed, 1.f, true, 0.f);
}


void ATpsGameMode::SpawnBotTimerElapsed()
{
	// Spawn new bot
	SpawnNewBot();

	// Decrement number of bots to spawn and end wave if it's zero
	if (--NumOfBotsToSpawn <= 0)
	{
		EndWave();
	}
}


void ATpsGameMode::EndWave()
{
	// Clear the spawn bots timer
	GetWorldTimerManager().ClearTimer(TimerHandle_SpawnBots);
}


void ATpsGameMode::PrepareForNextWave()
{
	// Simply set timer to start next wave
	GetWorldTimerManager().SetTimer(TimerHandle_PrepareWave, this, &ATpsGameMode::StartWave, TimeBetweenWaves, false);
}


void ATpsGameMode::CheckWaveState()
{
	// If still spawning new bots or preparing for the next wave: do nothing
	if (NumOfBotsToSpawn > 0 || GetWorldTimerManager().IsTimerActive(TimerHandle_PrepareWave)) 
		return;

	// keep track of if any bot is alive
	bool bAnyBotAlive = false;

	// Check if any bot is still alive by iterating through all bot pawns
	for (FConstPawnIterator it = GetWorld()->GetPawnIterator(); it; it++)
	{
		// Get the pawn that is alive and not controlled by the player
		APawn* TestPawn = it->Get();
		if (!TestPawn || TestPawn->IsPlayerControlled())
		{
			continue;
		}

		// Get the health component to see if it's still alive
		UTpsHealthComponent* PawnHealthComp = Cast<UTpsHealthComponent>(TestPawn->GetComponentByClass(UTpsHealthComponent::StaticClass()));
		if (PawnHealthComp && PawnHealthComp->GetCurrentHealth() > 0.f)
		{
			// If a bot is alive: break
			bAnyBotAlive = true;
			break;
		}
	}

	// if no bot is alive: prepare for the next wave
	if (!bAnyBotAlive)
	{
		PrepareForNextWave();
	}

}

