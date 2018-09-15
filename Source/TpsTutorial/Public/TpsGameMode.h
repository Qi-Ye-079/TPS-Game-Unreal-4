// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TpsGameMode.generated.h"

enum class EWaveState : uint8;

/**
 * 
 */
UCLASS()
class TPSTUTORIAL_API ATpsGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	// Constructor
	ATpsGameMode();
	
protected:
	// Spawn a single new bot (to be implemented in BP)
	UFUNCTION(BlueprintImplementableEvent, Category = "Blueprint Events")
	void SpawnNewBot();

	// Keep spawing the bot until no more bots to spawn
	void SpawnBotTimerElapsed();
	
	// Start spawning bots
	void StartWave();

	// End spawning bots
	void EndWave();

	// Set timer for nexr startwave
	void PrepareForNextWave();

	// Check the spawned bots status
	void CheckWaveState();

	// Function to check if any player is still alive
	void CheckPlayerState();

	// When no player is alive
	void GameOver();

	// Set the wave's state
	void SetWaveState(EWaveState NewState);

	// The timer handle to spawning new bots
	FTimerHandle TimerHandle_SpawnBots;

	// The timer handle to start the finextrst wave
	FTimerHandle TimerHandle_PrepareWave;

	// Number of bots to spawn in a single wave
	int NumOfBotsToSpawn;

	// The counter of wave
	int WaveCount;

	// Time between waves
	UPROPERTY(EditDefaultsOnly, Category = "Properties")
	float TimeBetweenWaves;

public:
	// Called when the game starts play
	virtual void StartPlay() override;

	// Called at specified interval
	virtual void Tick(float DeltaSeconds) override;


};
