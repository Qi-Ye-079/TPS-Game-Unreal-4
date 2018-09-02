// Fill out your copyright notice in the Description page of Project Settings.

#include "TpsProjectile.h"
#include "Gameframework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"


// Sets default values
ATpsProjectile::ATpsProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Make the static mesh block the Pawn only
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	MeshComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
	RootComponent = MeshComp;

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


void ATpsProjectile::Launch(const FVector& Velocity)
{
	ProjectileMoveComp->SetVelocityInLocalSpace(Velocity);
	ProjectileMoveComp->Activate();
	UE_LOG(LogTemp, Warning, TEXT("Launched: %s"), *Velocity.ToString());
}

