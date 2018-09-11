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
#include "TpsCharacter.h"

// Sets default values
ATpsWeapon::ATpsWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;


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


void ATpsWeapon::Fire(const FHitResult& HitResult)
{
	// If no mesh: simply return
	if (!MeshComp)
	{
		UE_LOG(LogTemp, Error, TEXT("This weapon has no mesh component! Please check BP."));
		return;
	}

	// 1. Get muzzle location and EndPoint and draw a debug line to help visualize the tracing line
	FVector MuzzleLocation = MeshComp->GetSocketLocation(TEXT("MuzzleFlashSocket"));
	FVector EndLocation = HitResult.bBlockingHit ? HitResult.ImpactPoint : HitResult.TraceEnd;
	DrawDebugLine(GetWorld(), MuzzleLocation, EndLocation, FColor::Red, false, 1.f, 0, 1.f);

	/*
	// 3. Spawn and launch projectile
	FActorSpawnParameters ProjectileSpawnParams;
	ProjectileSpawnParams.Instigator = Cast<ATpsCharacter>(GetOwner());
	ProjectileSpawnParams.Owner = this;
	ProjectileSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	ATpsProjectile* SpawnedProjectile = GetWorld()->SpawnActor<ATpsProjectile>(ProjectileClass, MuzzleLocation, LaunchDirection.Rotation(), ProjectileSpawnParams);
	if (SpawnedProjectile)
	{
		SpawnedProjectile->Launch(InitialBulletSpeed);
	}
	*/

	// 2. Apply damage to the hit point if the "ray" is blocked (instead of being overlapped or ignored)
	FVector HitFromDirection = (EndLocation - MuzzleLocation).GetSafeNormal();
	if (HitResult.bBlockingHit)
	{
		UGameplayStatics::ApplyPointDamage(HitResult.GetActor(), BaseDamage, HitFromDirection, HitResult, GetInstigatorController(), this, nullptr);
		//UE_LOG(LogTemp, Error, TEXT("Hit actor: %s by %f"), *HitResult.GetActor()->GetName(), damage);
	}

	// 4. Apply particle effect on the muzzle
	if (MuzzleEffect)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComp, TEXT("MuzzleFlashSocket"));
	}

	// 5. Apply smoke effect
	if (TracerEffect)
	{
		UParticleSystemComponent* TracerBeam = UGameplayStatics::SpawnEmitterAtLocation(this, TracerEffect, MuzzleLocation);
		if (TracerBeam)
		{
			//const FVector& EndPoint = IsHit ? HitRes.ImpactPoint : EndLocation;
			TracerBeam->SetVectorParameter("BeamEnd", EndLocation);
		}
	}
	
}

FORCEINLINE bool ATpsWeapon::IsAutomatic() const
{
	return bAuto;
}

FORCEINLINE float ATpsWeapon::GetFireRatePerSecond() const
{
	return FireRatePerSecond;
}

