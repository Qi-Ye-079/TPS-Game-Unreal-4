// Fill out your copyright notice in the Description page of Project Settings.

#include "TpsTrackerBot.h"
#include "GameFramework/Character.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "AI/Navigation/NavigationSystem.h"
#include "AI/Navigation/NavigationPath.h"


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

FVector ATpsTrackerBot::GetNextPathPoint()
{
	// Get the main player's location
	ACharacter *PlayerPawn = UGameplayStatics::GetPlayerCharacter(this, 0);

	// Get the navigation path
	UNavigationPath *NavPath = UNavigationSystem::FindPathToActorSynchronously(this, GetActorLocation(), PlayerPawn);

	// Get the next path point if path points is greater than 1
	if (NavPath->PathPoints.Num() > 1)
	{
		// Return the next point
		return NavPath->PathPoints[1];
	}
	// Failed to find next path point
	else
	{
		// Return current point
		return GetActorLocation();
	}
}

// Called every frame
void ATpsTrackerBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

