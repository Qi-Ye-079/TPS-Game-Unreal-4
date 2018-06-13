// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "TpsTrackerBot.generated.h"

UCLASS()
class TPSTUTORIAL_API ATpsTrackerBot : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ATpsTrackerBot();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	class UStaticMeshComponent *StaticMeshComp;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};
