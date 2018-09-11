// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyAIController.h"
#include "AI/EnemyAI.h"



void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();

	ControlledEnemyAI = Cast<AEnemyAI>(GetPawn());
}
