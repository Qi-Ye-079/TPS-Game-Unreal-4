// Fill out your copyright notice in the Description page of Project Settings.

#include "TpsGameMode.h"
#include "TimerManager.h"
#include "TpsGameState.h"
#include "TpsPlayerState.h"
#include "Components/TpsHealthComponent.h"


ATpsGameMode::ATpsGameMode()
	:TimeBetweenWaves(2.f)
{
	// Make this mode class tick every 1 second
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.f;

	// Set up game state class
	GameStateClass = ATpsGameState::StaticClass();
	PlayerStateClass = ATpsPlayerState::StaticClass();
}


void ATpsGameMode::StartPlay()
{
	Super::StartPlay();

	// Bind the On Actor Killed event
	// OnActorKilled.AddDynamic(this, &ATpsGameMode::HandleActorKilled);

	// Prepare for a wave
	PrepareForNextWave();
}


void ATpsGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Check wave and player status
	CheckWaveState();
	CheckPlayerState();
}


void ATpsGameMode::StartWave()
{
	// Increment wave count at beginning of each wave and set the number of bots to spawn
	NumOfBotsToSpawn = 2 * (++WaveCount);

	// Set the timer to spawn bots periodically
	GetWorldTimerManager().SetTimer(TimerHandle_SpawnBots, this, &ATpsGameMode::SpawnBotTimerElapsed, 1.f, true, 0.f);

	// Set the wave state to Wave in progress
	SetWaveState(EWaveState::WaveInProgress);
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

	// Set the wave state to waiting to complete
	SetWaveState(EWaveState::WaitingToComplete);
}


void ATpsGameMode::PrepareForNextWave()
{
	// Simply set timer to start next wave
	GetWorldTimerManager().SetTimer(TimerHandle_PrepareWave, this, &ATpsGameMode::StartWave, TimeBetweenWaves, false);

	// Set the wave state to Wave in progress
	SetWaveState(EWaveState::WaitingToStart);
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
		SetWaveState(EWaveState::WaveComplete);

		PrepareForNextWave();
	}

}


void ATpsGameMode::CheckPlayerState()
{
	for (FConstPlayerControllerIterator it = GetWorld()->GetPlayerControllerIterator(); it; it++)
	{
		APlayerController* PC = it->Get();
		if (PC && PC->GetPawn())
		{
			// Get player pawn and the health component to see if it's still alive
			APawn* PlayerPawn = PC->GetPawn();
			UTpsHealthComponent* PawnHealthComp = Cast<UTpsHealthComponent>(PlayerPawn->GetComponentByClass(UTpsHealthComponent::StaticClass()));
			if (ensure(PawnHealthComp) && PawnHealthComp->GetCurrentHealth() > 0.f)
			{
				return;
			}
		}	
	}

	// If no player is alive: game over
	GameOver();
}


void ATpsGameMode::GameOver()
{
	EndWave();

	// @TODO: end the game and present "game over" to the player

	SetWaveState(EWaveState::GameOver);

	UE_LOG(LogTemp, Log, TEXT("GAME OVER!!"));
}


void ATpsGameMode::SetWaveState(EWaveState NewState)
{
	ATpsGameState* GS = GetGameState<ATpsGameState>();
	if (ensureAlways(GS))
	{
		GS->SetWaveState(NewState);
	}
}

