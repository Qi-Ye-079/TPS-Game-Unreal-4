// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickupActor.generated.h"

// Forward declarations
class USphereComponent;
class UDecalComponent;

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

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USphereComponent* SphereComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UDecalComponent* DecalComp;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called when overlapping with other actors (overriding from Actor.h)
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	
};
