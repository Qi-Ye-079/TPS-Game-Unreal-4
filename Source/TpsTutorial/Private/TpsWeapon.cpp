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
#include <PhysicalMaterials/PhysicalMaterial.h>
#include <Particles/ParticleSystemComponent.h>

// Definitions of surface types
#define Flesh_default     SurfaceType1
#define Flesh_Vulnerable  SurfaceType2

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


//************************************
// Method:    Fire
// FullName:  ATpsWeapon::Fire
// Access:    protected 
// Returns:   void
// Qualifier:
// Parameter: HitRes - the reference to the HitResult
//************************************
void ATpsWeapon::Fire(const bool& IsHit, const FHitResult& HitRes, const FVector& TracingEndLocation)
{
	// 1. Get muzzle location and EndPoint; draw a debug line to help visualize the tracing line
	FVector MuzzleLocation = MeshComp->GetSocketLocation(TEXT("MuzzleFlashSocket"));
	FVector EndPoint = IsHit ? HitRes.ImpactPoint : TracingEndLocation;
	//DrawDebugLine(GetWorld(), MuzzleLocation, EndPoint, FColor::Red, false, 1.f, 0, 1.f);

	if (MeshComp)
	{
		// 2. Apply particle effect on the muzzle
		if (MuzzleEffect)
		{
			UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComp, TEXT("MuzzleFlashSocket"));
		}

		// 3. Apply smoke effect
		if (TracerEffect)
		{
			FVector MuzzleLocation = MeshComp->GetSocketLocation(TEXT("MuzzleFlashSocket"));
			UParticleSystemComponent* TracerBeam = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TracerEffect, MuzzleLocation);
			if (TracerBeam)
			{
				//const FVector& EndPoint = IsHit ? HitRes.ImpactPoint : EndLocation;
				TracerBeam->SetVectorParameter("BeamEnd", EndPoint);
			}
		}
	}

	// 4. Handle impact effect and damage
	float ActualDamage = BaseDamage;
	// Select the impact effect type
	if (IsHit)
	{
		EPhysicalSurface HitPhysicalSurf = UPhysicalMaterial::DetermineSurfaceType(HitRes.PhysMaterial.Get());
		UParticleSystem* CurrentImpactEffect = nullptr;

		switch (HitPhysicalSurf)
		{
		case Flesh_Vulnerable:
			// If head shot: 5x more damage
			ActualDamage *= 5.f;
		case Flesh_default:
			CurrentImpactEffect = FleshImpactEffect;
			break;
		default:
			CurrentImpactEffect = DefaultImpactEffect;
			break;
		}

		// Spawn the blood effect on the hit actor
		if (CurrentImpactEffect)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), CurrentImpactEffect, HitRes.ImpactPoint, HitRes.ImpactNormal.Rotation());
		}
	}

	// 5. Get the player controller who shot the weapon and apply damage to the actor who is hit
	AController* EventInstigator = GetOwner()->GetInstigatorController();
	if (EventInstigator && IsHit)
	{
		const FVector HitFromDirection = EndPoint - MuzzleLocation;
		UGameplayStatics::ApplyPointDamage(HitRes.GetActor(), ActualDamage, HitFromDirection, HitRes, EventInstigator, this, DamageType);
		DrawDebugString(GetWorld(), HitRes.ImpactPoint, FString::SanitizeFloat(ActualDamage), nullptr, FColor::White, 2.f);
	}
}

// Called every frame
void ATpsWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

