// Fill out your copyright notice in the Description page of Project Settings.

#include "Rifle.h"
#include <Kismet/GameplayStatics.h>
#include <Components/SkeletalMeshComponent.h>


ARifle::ARifle()
{
	// Empty since the parent already does everything
}

void ARifle::BeginPlay()
{
	Super::BeginPlay();

}

//************************************
// Method:    Fire
// FullName:  ARifle::Fire
// Access:    protected 
// Returns:   void
// Qualifier:
// Parameter: hit - the reference to the HitResult
//************************************
void ARifle::Fire(const FHitResult& hit)
{
	// Draw a debug line to help visualize the tracing line
	FVector MuzzleLocation = MeshComp->GetSocketLocation(TEXT("MuzzleFlashSocket"));
	FVector HitLocation = hit.ImpactPoint;
	//DrawDebugLine(GetWorld(), MuzzleLocation, HitLocation, FColor::Red, false, 1.f, 0, 1.f);

	// Apply particle effect on the muzzle
	if (MuzzleEffect)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComp, TEXT("MuzzleFlashSocket"));
	}

	// Get the player controller who shot the weapon and apply damage to the actor who is hit
	AController* EventInstigator = GetOwner()->GetInstigatorController();
	if (EventInstigator)
	{
		UGameplayStatics::ApplyPointDamage(hit.GetActor(), 20.f, (HitLocation - MuzzleLocation), hit, EventInstigator, this, DamageType);
	}

	// Spawn the blood effect on the hit actor
	if (ImpactEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, hit.ImpactPoint, hit.ImpactNormal.Rotation());
	}
}

void ARifle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
