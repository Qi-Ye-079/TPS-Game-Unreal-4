// Fill out your copyright notice in the Description page of Project Settings.

#include "PickupActor.h"
#include "PowerupActor.h"
#include "Components/SphereComponent.h"
#include "Components/DecalComponent.h"
#include "TimerManager.h"


// Sets default values
APickupActor::APickupActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetSphereRadius(75.f);
	RootComponent = SphereComp;

	DecalComp = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComp"));
	DecalComp->SetRelativeRotation(FRotator(90.f, 0.f, 0.f));
	DecalComp->DecalSize = FVector(64.f, 75.f, 75.f);
	DecalComp->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void APickupActor::BeginPlay()
{
	Super::BeginPlay();

	// Spawn powerups
	SpawnPowerup();
	
}


void APickupActor::SpawnPowerup()
{
	// If no powerup class, simply return
	if (!PowerupClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("No power up class specified in %s. Please check it out in blueprint."), *GetName());
		return;
	}

	// Spawn the power up actor
	FActorSpawnParameters PowerupSpawnParams;
	PowerupSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	PowerupInstance = GetWorld()->SpawnActor<APowerupActor>(PowerupClass, GetTransform(), PowerupSpawnParams);
}

void APickupActor::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	// Grant the player a PowerUp if available
	if (PowerupInstance)
	{
		PowerupInstance->ActivatePowerup();
		PowerupInstance = nullptr;

		// Set a timer to respawn the powerup
		// GetWorldTimerManager().SetTimer(TimerHandle_SpawnPowerup, this, &APickupActor::SpawnPowerup, CooldownTime);
	}
}

