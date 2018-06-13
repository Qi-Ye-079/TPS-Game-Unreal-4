// Fill out your copyright notice in the Description page of Project Settings.

#include "TpsTrackerBot.h"
#include "Components/StaticMeshComponent.h"


// Sets default values
ATpsTrackerBot::ATpsTrackerBot()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create static mesh component
	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComp"));
	StaticMeshComp->SetCanEverAffectNavigation(false);
	RootComponent = StaticMeshComp;
}

// Called when the game starts or when spawned
void ATpsTrackerBot::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATpsTrackerBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

