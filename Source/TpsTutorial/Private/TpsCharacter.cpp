// Fill out your copyright notice in the Description page of Project Settings.

#include "TpsCharacter.h"
#include <Components/InputComponent.h>
#include <Components/SkeletalMeshComponent.h>
#include <Camera/CameraComponent.h>
#include <GameFramework/SpringArmComponent.h>
#include <GameFramework/CharacterMovementComponent.h>
#include <DrawDebugHelpers.h>
#include <Kismet/GameplayStatics.h>
#include "../Public/TpsCharacter.h"
#include "TpsWeapon.h"


// Sets default values
ATpsCharacter::ATpsCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// When rotate the camera: don't rotate the character
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;

	// Set the following to enable the character to crouch (??weired setting)
	UCharacterMovementComponent* CharMoveComp = GetCharacterMovement();
	CharMoveComp->GetNavAgentPropertiesRef().bCanCrouch = true;
	CharMoveComp->bOrientRotationToMovement = true;
	CharMoveComp->MaxWalkSpeed = 450.f;

	// Auto posses player 0
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	// Create a Spring Arm Component
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(RootComponent);           // Must-have
	SpringArmComp->bUsePawnControlRotation = true;           // Must-have
	SpringArmComp->TargetArmLength = 250.f;                  // length of the spring arm
	SpringArmComp->SocketOffset = FVector(0.f, 60.f, 40.f);  // Offset at the end of spring arm(socket)
	SpringArmComp->bEnableCameraLag = true;
	SpringArmComp->CameraLagSpeed = 10.f;
	//SpringArmComp->SetRelativeLocationAndRotation(FVector(15.f, 0.f, 0.f), FRotator(0.f, -60.f, 0.f));

	// Create a UCameraComponent for this player.
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);
}

// Called when the game starts or when spawned
void ATpsCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Spawn weapon
	if (WeaponClass)
	{
		// Get transformation of spawned actor (values not important because it will be attached 
		// to character's weapon socket later
		FTransform transform;

		// Get the spawn parameters
		FActorSpawnParameters params;
		params.Owner = this;
		params.Instigator = Instigator;

		// Spawn the actor (weapon)
		currentWeapon = Cast<ATpsWeapon>(GetWorld()->SpawnActor(WeaponClass, &transform, params));
		if (currentWeapon && GetMesh()->GetSocketByName(TEXT("WeaponSocket")))
		{
			// Location Rule, Rotation Rule and Scale Rule all set to Snap to target
			// And Weld Simulated Bodies set to true
			FAttachmentTransformRules rules(EAttachmentRule::SnapToTarget, true);

			// Attach weapon to the WeaponSocket
			currentWeapon->AttachToComponent(GetMesh(), rules, TEXT("WeaponSocket"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("No socket named WeaponSocket!!! Please create one."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("No Weapon class assigned. Pls do so in BP editor."));
	}

}

void ATpsCharacter::MoveForward(float axisValue)
{
	// Get the direction of the controller
	const FRotator ControllerRotation = GetControlRotation();

	// Get the forward vector
	const FVector forward = FRotationMatrix(ControllerRotation).GetUnitAxis(EAxis::X);

	// Apply the forward vector to input
	AddMovementInput(forward, axisValue);
}

void ATpsCharacter::MoveRight(float axisValue)
{
	// Get the direction to the right of the controller
	const FRotator ControllerRotation = GetControlRotation();

	// Get the right vector
	const FVector right = FRotationMatrix(ControllerRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(right, axisValue);
}

void ATpsCharacter::beginCrouch()
{
	Crouch(); // Built-in function
}

void ATpsCharacter::endCrouch()
{
	UnCrouch(); // Built-in function
}

// Called every frame
void ATpsCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ATpsCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Bind forward and right input axis to the corresponding functions.
	PlayerInputComponent->BindAxis("MoveForward", this, &ATpsCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ATpsCharacter::MoveRight);

	// Bind camera input
	PlayerInputComponent->BindAxis("Lookup", this, &ATpsCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn", this, &ATpsCharacter::AddControllerYawInput);
	// Then go to UE4 Project Settings -> Input -> axis mappings to create axis inputs.

	// Bind actions(NOTE: cannot directly use Crouch() and UnCrouch() function here. Why??)
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ATpsCharacter::beginCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ATpsCharacter::endCrouch);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ATpsCharacter::Fire);
}

//************************************
// Method:    Fire
// FullName:  ATpsCharacter::Fire
// Access:    public 
// Returns:   void
// Qualifier: 
// Parameter: None
//************************************
void ATpsCharacter::Fire()
{
	// First we need to line trace from the view of camera
	// Get the location and rotation of from camera's view
	FVector camLocation;
	FRotator camRotation;
	//SpringArmComp->GetSocketWorldLocationAndRotation(USpringArmComponent::SocketName, camLocation, camRotation);
	CameraComp->GetSocketWorldLocationAndRotation(USpringArmComponent::SocketName, camLocation, camRotation);
	// Get the end location of tracing line with a large distance
	FVector& startLocation = camLocation;
	FVector  endLocation   = camLocation + camRotation.Vector() * 10000.f;

	// Better to specify the Collision Query Parameters as well
	// For a precise hit point; more costly but looks way more natural
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(currentWeapon);
	QueryParams.AddIgnoredActor(this);
	QueryParams.bTraceComplex = true;

	// Do line tracing by channel (ECC_Visibility: hit anything visible that blocks the line)
	// Note that the hit actor's collision should be enabled, especially the traced channel
	FHitResult hit; // The HitResult struct
	if (currentWeapon && 
		GetWorld()->LineTraceSingleByChannel(hit, startLocation, endLocation, ECC_Visibility, QueryParams))
	{
		// If blocking hit happens: get the location of weapon muzzle and hit point
		USkeletalMeshComponent* WeaponSkeletalMesh = currentWeapon->getSkeletalMesh();
		if (WeaponSkeletalMesh)
		{
			FVector muzzleLocation = WeaponSkeletalMesh->GetSocketLocation(TEXT("MuzzleFlashSocket"));
			FVector hitLocation = hit.ImpactPoint;
			// Draw a debug line to help visualize the tracing line
			DrawDebugLine(GetWorld(), muzzleLocation, hitLocation, FColor::Red, false, 1.f, 0, 1.f);

			// Get the player controller who shot the weapon
			AController* eventInstigator = GetInstigatorController();
			// Apply damage to the actor who is hit
			UGameplayStatics::ApplyPointDamage(hit.GetActor(), 20.f, (hitLocation - muzzleLocation), hit, eventInstigator, this, DamageType);
		}
	}
}

