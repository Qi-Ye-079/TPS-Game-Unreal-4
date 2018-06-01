// Fill out your copyright notice in the Description page of Project Settings.

#include "Rifle.h"
#include <Kismet/GameplayStatics.h>
#include <Components/SkeletalMeshComponent.h>

ARifle::ARifle()
{
	// Set up 3 key properties
	bAuto = true;
	BaseDamage = 20.f;
	FireRatePerSecond = 10.f;
}

void ARifle::BeginPlay()
{
	Super::BeginPlay();

}

void ARifle::Fire(const bool& IsHit, const FHitResult& HitRes, const FVector& TracingEndLocation)
{
	Super::Fire(IsHit, HitRes, TracingEndLocation);

}

void ARifle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
