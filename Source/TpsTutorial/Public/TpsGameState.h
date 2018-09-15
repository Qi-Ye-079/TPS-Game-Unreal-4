// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "TpsGameState.generated.h"

UENUM(BlueprintType)
enum class EWaveState : uint8
{
	WaitingToStart,

	WaveInProgress,

	// No longer spawning new bots, waiting for player to kill all remaining bots
	WaitingToComplete,

	WaveComplete,

	// No player is alive
	GameOver
};

/**
 * 
 */
UCLASS()
class TPSTUTORIAL_API ATpsGameState : public AGameStateBase
{
	GENERATED_BODY()
	
protected:
	UFUNCTION()
	void OnRep_WaveState(EWaveState OldState);

	// Event to fire when wave state has changed (implemented in BP)
	UFUNCTION(BlueprintImplementableEvent, Category = "Blueprint Events")
	void WaveStateChanged(EWaveState NewState, EWaveState OldState);


	// Current wave state in game
	UPROPERTY(BlueprintReadOnly, Category = "Game State")
	EWaveState WaveState;

public:
	// Getters and setters to the WaveState variable
	EWaveState GetWaveState() const;
	void SetWaveState(EWaveState NewState);
	
	
};