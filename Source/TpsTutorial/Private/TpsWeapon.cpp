// Fill out your copyright notice in the Description page of Project Settings.

#include "TpsWeapon.h"
#include <Components/SkeletalMeshComponent.h>
#include <CollisionQueryParams.h>
#include <Engine/World.h>
#include <Camera/CameraComponent.h>
#include <GameFramework/SpringArmComponent.h>
#include <DrawDebugHelpers.h>
#include <Kismet/GameplayStatics.h>
#include <Particles/ParticleSystem.h>


// Sets default values
ATpsWeapon::ATpsWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create the skeletal mesh and make it root component
	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;
}

// Called when the game starts or when spawned
void ATpsWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATpsWeapon::Fire(const FHitResult& hit)
{
	// Draw a debug line to help visualize the tracing line
	FVector MuzzleLocation = MeshComp->GetSocketLocation(TEXT("MuzzleFlashSocket"));
	FVector HitLocation = hit.ImpactPoint;
	DrawDebugLine(GetWorld(), MuzzleLocation, HitLocation, FColor::Red, false, 1.f, 0, 1.f);
	
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

// Called every frame
void ATpsWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

