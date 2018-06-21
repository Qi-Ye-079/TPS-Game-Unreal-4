// Fill out your copyright notice in the Description page of Project Settings.

#include "TpsTrackerBot.h"
#include "GameFramework/Character.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "AI/Navigation/NavigationSystem.h"
#include "AI/Navigation/NavigationPath.h"
#include "DrawDebugHelpers.h"


// Sets default values
ATpsTrackerBot::ATpsTrackerBot()
	:ForceMag(50000.f)
	,bAccelChangeInVelocity(false)
	,RequiredDistanceToTarget(100.f)
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create static mesh component
	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComp"));
	StaticMeshComp->SetCanEverAffectNavigation(false);
	StaticMeshComp->SetSimulatePhysics(true);  // Must-have for AddForce to work!!!
	RootComponent = StaticMeshComp;
}

// Called when the game starts or when spawned
void ATpsTrackerBot::BeginPlay()
{
	Super::BeginPlay();

	// Find initial next path point
	NextPathPoint = GetNextPathPoint();
	
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

	// Current location of this Tracker bot
	FVector CurrentPoint = GetActorLocation();

	// Get the distance between this and target
	float DistanceToTarget = (NextPathPoint - CurrentPoint).Size();

	// If close enough to the target
	if (DistanceToTarget <= RequiredDistanceToTarget)
	{
		NextPathPoint = GetNextPathPoint();
	}
	else
	{
		// Get the direction to Force to apply
		FVector ForceDir = NextPathPoint - CurrentPoint;
		ForceDir.Normalize();

		// Set magnitude of force
		ForceDir *= ForceMag;

		// Apply force to this tracker bot
		StaticMeshComp->AddForce(ForceDir, NAME_None, bAccelChangeInVelocity);
	}

	// Add debug sphere to help visualize the next point
	DrawDebugDirectionalArrow(GetWorld(), CurrentPoint, NextPathPoint, 40.f, FColor::Yellow);
	DrawDebugSphere(GetWorld(), NextPathPoint, 20.f, 10.f, FColor::Yellow);
}

