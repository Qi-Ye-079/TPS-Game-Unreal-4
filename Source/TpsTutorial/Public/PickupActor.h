// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickupActor.generated.h"

// Forward declarations
class USphereComponent;
class UDecalComponent;
class APowerupActor;

UCLASS()
class TPSTUTORIAL_API APickupActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickupActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void SpawnPowerup();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* SphereComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UDecalComponent* DecalComp;

	// The power up actor to spawn
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Properties")
	TSubclassOf<APowerupActor> PowerupClass;

	// The spawned powerup actor instance
	APowerupActor* PowerupInstance;

	// The cooldown time after spawning a new powerup
	UPROPERTY(EditDefaultsOnly, Category = "Properties")
	float CooldownTime;

	FTimerHandle TimerHandle_SpawnPowerup;

public:	

	// Called when overlapping with other actors (overriding from Actor.h)
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	
};
