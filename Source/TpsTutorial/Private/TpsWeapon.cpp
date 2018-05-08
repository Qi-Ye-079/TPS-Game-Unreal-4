// Fill out your copyright notice in the Description page of Project Settings.

#include "TpsWeapon.h"
#include <CollisionQueryParams.h>
#include <Engine/World.h>
#include <DrawDebugHelpers.h>


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

void ATpsWeapon::ShootProjectile()
{
	// First we need to line trace from the pawn eyes to the crosshair location (screen center)
	AActor* myOwner = GetOwner();
	if (myOwner)
	{
		// Get the location and rotation of the eye view(from the camera)
		FVector camLocation;
		FRotator camRotation;
		myOwner->GetActorEyesViewPoint(camLocation, camRotation);
		// Get the end location of tracing line with distance 10000
		FVector endLocation = camLocation + camRotation.Vector() * 10000.f;

		// Better to specify the Collision Query Parameters as well
		// Because it will get a precise hit point
		// More costly but way more natural
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(myOwner);
		QueryParams.AddIgnoredActor(this);
		QueryParams.bTraceComplex = true;

		// The HitResult struct
		FHitResult hit;
		// Do line tracing by channel (ECC_Visibility: hit anything visible that blocks the line)
		if (GetWorld()->LineTraceSingleByChannel(hit, camLocation, endLocation, ECC_Visibility, QueryParams))
		{
			// If blocking hit happens
		}

		// Draw a debug line to help us visualize the tracing line
		DrawDebugLine(GetWorld(), camLocation, endLocation, FColor::Red, false, 1.f, 0, 1.f);
	}
	
}

// Called every frame
void ATpsWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

