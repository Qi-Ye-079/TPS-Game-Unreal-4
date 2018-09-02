// Fill out your copyright notice in the Description page of Project Settings.

#include "TpsWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "CollisionQueryParams.h"
#include "Engine/World.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Particles/ParticleSystemComponent.h"
#include "TpsProjectile.h"

// Sets default values
ATpsWeapon::ATpsWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


}

// Called when the game starts or when spawned
void ATpsWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	
}


void ATpsWeapon::SetStaticMeshComponent(UStaticMeshComponent* StaticMeshComp)
{
	MeshComp = StaticMeshComp;
}

// Called every frame
void ATpsWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void ATpsWeapon::Fire(const FVector& EndLocation)
{
	// If no mesh: simply return
	if (!MeshComp)
	{
		UE_LOG(LogTemp, Error, TEXT("This weapon has no mesh component! Please check BP."));
		return;
	}

	// 1. Get muzzle location and EndPoint; draw a debug line to help visualize the tracing line
	FVector MuzzleLocation = MeshComp->GetSocketLocation(TEXT("MuzzleFlashSocket"));

	// UE_LOG(LogTemp, Error, TEXT("Muzzle socket location: %s"), *MuzzleLocation.ToString());
	// FVector EndPoint = bHit ? HitRes.ImpactPoint : EndLocation;
	DrawDebugLine(GetWorld(), MuzzleLocation, EndLocation, FColor::Red, false, 1.f, 0, 1.f);

	// 2. Get the rotation of launching projectile
	FVector LaunchDirection = (EndLocation - MuzzleLocation).GetSafeNormal();

	// 3. Spawn and launch projectile
	FActorSpawnParameters ProjectileSpawnParams;
	ProjectileSpawnParams.Instigator = Instigator;
	ProjectileSpawnParams.Owner = this;
	ProjectileSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	ATpsProjectile* SpawnedProjectile = GetWorld()->SpawnActor<ATpsProjectile>(ProjectileClass, MuzzleLocation, LaunchDirection.Rotation(), ProjectileSpawnParams);
	if (SpawnedProjectile)
	{
		SpawnedProjectile->Launch(InitialBulletSpeed);
	}

	// 2. Apply particle effect on the muzzle
	if (MuzzleEffect)
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComp, TEXT("MuzzleFlashSocket"));

	// 3. Apply smoke effect
	if (TracerEffect)
	{
		UParticleSystemComponent* TracerBeam = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TracerEffect, MuzzleLocation);
		if (TracerBeam)
		{
			//const FVector& EndPoint = IsHit ? HitRes.ImpactPoint : EndLocation;
			TracerBeam->SetVectorParameter("BeamEnd", EndLocation);
		}
	}

	/*
	// 4. Handle impact effect and damage
	float ActualDamage = BaseDamage;
	// Select the impact effect type
	if (bHit)
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
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), CurrentImpactEffect, HitRes.ImpactPoint, HitRes.ImpactNormal.Rotation());
	}

	// 5. Get the player controller who shot the weapon and apply damage to the actor who is hit
	AController* EventInstigator = GetOwner()->GetInstigatorController();
	if (EventInstigator && bHit)
	{
		const FVector HitFromDirection = EndPoint - MuzzleLocation;
		UGameplayStatics::ApplyPointDamage(HitRes.GetActor(), ActualDamage, HitFromDirection, HitRes, EventInstigator, this, DamageType);
		DrawDebugString(GetWorld(), HitRes.ImpactPoint, FString::SanitizeFloat(ActualDamage), nullptr, FColor::White, 2.f);
	}
	*/
	
}

bool ATpsWeapon::IsAutomatic() const
{
	return bAuto;
}

float ATpsWeapon::GetFireRatePerSecond() const
{
	return FireRatePerSecond;
}

