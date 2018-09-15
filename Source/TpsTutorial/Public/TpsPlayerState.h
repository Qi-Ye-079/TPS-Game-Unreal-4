// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "TpsPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class TPSTUTORIAL_API ATpsPlayerState : public APlayerState
{
	GENERATED_BODY()
	
	
public:
	UFUNCTION(BlueprintCallable, Category = "State")
	void AddScore(float ScoreDelta);
	
};
