// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

class AEnemyAI;

/**
 * 
 */
UCLASS()
class TPSTUTORIAL_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
protected:
	// The ref to the controlled enemy AI by this controller
	UPROPERTY(BlueprintReadOnly)
	AEnemyAI* ControlledEnemyAI;
};
