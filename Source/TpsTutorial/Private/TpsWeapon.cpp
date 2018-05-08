// Fill out your copyright notice in the Description page of Project Settings.

#include "TpsWeapon.h"
#include <Components/SkeletalMeshComponent.h>
#include <CollisionQueryParams.h>
#include <Engine/World.h>
#include <DrawDebugHelpers.h>
#include <Camera/CameraComponent.h>
#include <GameFramework/SpringArmComponent.h>


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
// Method:    ShootProjectile
// FullName:  ATpsWeapon::ShootProjectile
// Access:    public 
// Returns:   void
// Qualifier: 
// Parameter: CameraComp - the pointer to the main camera of the owner pawn
//************************************
void ATpsWeapon::ShootProjectile(UCameraComponent* CameraComp)
{
	// First we need to line trace from the view of camera
	AActor* myOwner = GetOwner();
	if (myOwner)
	{
		// Get the location and rotation of from camera's view
		FVector camLocation;
		FRotator camRotation;
		CameraComp->GetSocketWorldLocationAndRotation(USpringArmComponent::SocketName, camLocation, camRotation);
		// Get the end location of tracing line with a large distance
		FVector& startLocation =  camLocation;
		FVector  endLocation   =  camLocation + camRotation.Vector() * 10000.f;

		// Better to specify the Collision Query Parameters as well
		// Because it will get a precise hit point
		// More costly but way more natural
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(myOwner);
		QueryParams.AddIgnoredActor(this);
		QueryParams.bTraceComplex = true;

		// Do line tracing by channel (ECC_Visibility: hit anything visible that blocks the line)
		FHitResult hit; // The HitResult struct
		if (GetWorld()->LineTraceSingleByChannel(hit, startLocation, endLocation, ECC_Visibility, QueryParams))
		{
			// If blocking hit happens: get the location of the weapon muzzle
			FVector muzzleLocation = MeshComp->GetSocketLocation(TEXT("MuzzleFlashSocket"));

			// Then get the hit point
			FVector hitLocation = hit.ImpactPoint;

			// Draw a debug line to help visualize the tracing line
			DrawDebugLine(GetWorld(), muzzleLocation, hitLocation, FColor::Red, false, 1.f, 0, 1.f);
		}
	}
	
}

// Called every frame
void ATpsWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

