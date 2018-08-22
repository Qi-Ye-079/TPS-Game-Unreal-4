// Fill out your copyright notice in the Description page of Project Settings.

#include "TpsTrackerBot.h"
#include "TpsCharacter.h"
#include "Components/TpsHealthComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/DamageType.h"
#include "GameFramework/Controller.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundCue.h"
#include "GameFramework/Actor.h"


// Sets default values
ATpsTrackerBot::ATpsTrackerBot()
	:ForceMag(50000.f)
	,bAccelChangeInVelocity(false)
	,RequiredDistanceToTarget(100.f)
	,ExplodeBaseDamage(50.f)
	,ExplodeRadius(300.f)
	,SelfDamage(20.f)
	,SelfDamageInterval(0.5f)
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create static mesh component
	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComp"));
	StaticMeshComp->SetCanEverAffectNavigation(false);
	StaticMeshComp->SetSimulatePhysics(true);  // Must-have for AddForce to work!!!
	RootComponent = StaticMeshComp;

	// Use sphere component for collision test
	// Ignore all channels except for the ECC_Pawn channel.
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetSphereRadius(ExplodeRadius * 0.9f);
	SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	SphereComp->SetupAttachment(RootComponent);

	// Create health component
	HealthComp = CreateDefaultSubobject<UTpsHealthComponent>(TEXT("HealthComp"));
}

// Called when the game starts or when spawned
void ATpsTrackerBot::BeginPlay()
{
	Super::BeginPlay();

	// Subscribe to the On Health Changed event of Health Component.
	// Add dynamic in the BeginPlay instead of Constructor because this dynamic was added after
	// the creation of BP, and it wouldn't work to put it in the constructor(bug?).
	HealthComp->OnHealthChanged.AddDynamic(this, &ATpsTrackerBot::HandleOnTakeDamage);

	// Find initial next path point
	NextPathPoint = GetNextPathPoint();
}


FVector ATpsTrackerBot::GetNextPathPoint()
{
	// Get the main player's location
	ACharacter *PlayerPawn = UGameplayStatics::GetPlayerCharacter(this, 0);

	// Get the navigation path
	UNavigationPath *NavPath = UNavigationSystemV1::FindPathToActorSynchronously(this, GetActorLocation(), PlayerPawn);

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


void ATpsTrackerBot::Explode()
{
	// Spawn the explosion particle effect
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());

	// Apply radial damage to surroundings
	TArray<AActor*> IgnoredActors;
	IgnoredActors.Add(this);
	UGameplayStatics::ApplyRadialDamage(this, ExplodeBaseDamage, GetActorLocation(), ExplodeRadius, nullptr, IgnoredActors, this, GetInstigatorController());

	// Draw debug sphere
	DrawDebugSphere(GetWorld(), GetActorLocation(), ExplodeRadius, 12, FColor::Red, false, 2.f);

	// Destroy this tracker bot and play explosion sound
	Destroy();
	UGameplayStatics::PlaySoundAtLocation(this, ExplodeSoundEffect, GetActorLocation());
}


void ATpsTrackerBot::DamageSelf()
{
	// Damage itself
	UGameplayStatics::ApplyDamage(this, SelfDamage, GetInstigatorController(), this, nullptr);
}


void ATpsTrackerBot::HandleOnTakeDamage(UTpsHealthComponent *OwningHealthComp, float CurrentHealth, float HealthDelta, 
	const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	// Output log
	UE_LOG(LogTemp, Log, TEXT("Health %s of %s"), *FString::SanitizeFloat(CurrentHealth), *GetName());

	// If current health == 0: explode
	if (CurrentHealth <= 0)
	{
		Explode();
		return;
	}

	// Create emissive pulse on taking damage
	if (!MatInstance) 
		// Only create a new material instance when there's none
		MatInstance = StaticMeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(0, StaticMeshComp->GetMaterial(0));

	if (MatInstance) // Always make sure the material instance is not null because the above operation won't guarantee to succeed
		MatInstance->SetScalarParameterValue("LastTimeDamageTaken", GetWorld()->TimeSeconds);
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


void ATpsTrackerBot::NotifyActorBeginOverlap(AActor* OtherActor)
{
	// See if the otherActor is a TpsCharacter
	ATpsCharacter* PlayerCharacter = Cast<ATpsCharacter>(OtherActor);
	if (PlayerCharacter)
	{
		// If it's a TpsCharacter (Player): keep self damaging itself every 0.5 seconds
		GetWorldTimerManager().SetTimer(TimerHandle_SelfDamage, this, &ATpsTrackerBot::DamageSelf, SelfDamageInterval, true, 0.f);

		// Play sound effect
		UGameplayStatics::SpawnSoundAttached(SelfDamageWarningSoundEffect, RootComponent);
	}
}

