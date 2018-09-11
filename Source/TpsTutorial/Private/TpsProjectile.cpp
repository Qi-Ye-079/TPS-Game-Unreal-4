// Fill out your copyright notice in the Description page of Project Settings.

#include "TpsProjectile.h"
#include "Gameframework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"


// Sets default values
ATpsProjectile::ATpsProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ProjectileMoveComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMoveComp"));
	ProjectileMoveComp->bAutoActivate = false;



}

// Called when the game starts or when spawned
void ATpsProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATpsProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void ATpsProjectile::Launch(float LaunchSpeed)
{
	ProjectileMoveComp->SetVelocityInLocalSpace(FVector::ForwardVector * LaunchSpeed);
	ProjectileMoveComp->Activate();
}

