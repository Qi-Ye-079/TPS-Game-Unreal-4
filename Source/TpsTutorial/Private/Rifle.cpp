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

void ARifle::Fire(const bool& IsHit, const FHitResult& HitRes, const FVector& TracingEndLocation)
{
	Super::Fire(IsHit, HitRes, TracingEndLocation);

}

void ARifle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
