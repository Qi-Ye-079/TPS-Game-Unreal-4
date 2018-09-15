// Fill out your copyright notice in the Description page of Project Settings.

#include "TpsGameState.h"




void ATpsGameState::OnRep_WaveState(EWaveState OldState)
{
	WaveStateChanged(WaveState, OldState);
}


EWaveState ATpsGameState::GetWaveState() const
{
	return WaveState;
}


void ATpsGameState::SetWaveState(EWaveState NewState)
{
	if (NewState == WaveState)
		return;

	// Set wave state and fire event on wave state changed
	EWaveState OldState = WaveState;
	WaveState = NewState;
	WaveStateChanged(NewState, OldState);
}
