// Fill out your copyright notice in the Description page of Project Settings.

#include "TpsCharacter.h"
#include <Components/InputComponent.h>
#include <Components/SkeletalMeshComponent.h>
#include <Camera/CameraComponent.h>
#include <GameFramework/SpringArmComponent.h>
#include <GameFramework/CharacterMovementComponent.h>
#include <Kismet/GameplayStatics.h>
#include <Particles/ParticleSystem.h>
#include "Components/CapsuleComponent.h"
#include "../Public/TpsCharacter.h"
#include <TimerManager.h>


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
	CameraComp->SetFieldOfView(DefaultFov);

	// Make the Capsule component ignore the Weapon Collision trace channel
	// since we only want the Mesh Component to respond to that channel
	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);

	// Auto posses player 0
	AutoPossessPlayer = EAutoReceiveInput::Player0;
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
		CurrentWeapon = Cast<ATpsWeapon>(GetWorld()->SpawnActor(WeaponClass, &transform, params));
		if (CurrentWeapon && GetMesh()->GetSocketByName(TEXT("WeaponSocket")))
		{
			// Location Rule, Rotation Rule and Scale Rule all set to Snap to target
			// And Weld Simulated Bodies set to true
			FAttachmentTransformRules rules(EAttachmentRule::SnapToTarget, true);

			// Attach weapon to the WeaponSocket
			CurrentWeapon->AttachToComponent(GetMesh(), rules, TEXT("WeaponSocket"));
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

void ATpsCharacter::BeginCrouch()
{
	Crouch(); // Built-in function
}

void ATpsCharacter::EndCrouch()
{
	UnCrouch(); // Built-in function
}

void ATpsCharacter::StartShoot()
{
	// Check if the weapon is automatic and get its fire rate
	bool Loop = CurrentWeapon->IsAutomatic();
	float FirePeriod = 1.f / (CurrentWeapon->GetFireRatePerSecond());

	// Determine the delay of pressing the mouse
	float FirstDelay = FMath::Max(LastFireTime + FirePeriod - GetWorld()->TimeSeconds, 0.f);

	// Keeps shooting weapon at the desired rate
	GetWorldTimerManager().SetTimer(TimerHandle, this, &ATpsCharacter::ShootWeapon, FirePeriod, Loop, FirstDelay);
}

void ATpsCharacter::EndShoot()
{
	// Clear the timer handler
	GetWorldTimerManager().ClearTimer(TimerHandle);
}

void ATpsCharacter::ZoomIn()
{
	ZoomingIn = true;
	bUseControllerRotationYaw = true;
}

void ATpsCharacter::ZoomOut()
{
	ZoomingIn = false;
	bUseControllerRotationYaw = false;
}

// Called every frame
void ATpsCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);	

	// Handle zoom in/out
	if (ZoomingIn)
	{
		ZoomAlpha += DeltaTime * ZoomSpeed;
	}
	else
	{
		ZoomAlpha -= DeltaTime * ZoomSpeed;
	}
	// Clamp the ZoomAlpha value between 0 ~ 1.0
	ZoomAlpha = FMath::Clamp(ZoomAlpha, 0.f, 1.f);
	// Update FOV
	CameraComp->SetFieldOfView(FMath::Lerp(DefaultFov, ZoomInFov, ZoomAlpha));

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
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ATpsCharacter::BeginCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ATpsCharacter::EndCrouch);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ATpsCharacter::StartShoot);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ATpsCharacter::EndShoot);
	PlayerInputComponent->BindAction("Zoom", IE_Pressed, this, &ATpsCharacter::ZoomIn);
	PlayerInputComponent->BindAction("Zoom", IE_Released, this, &ATpsCharacter::ZoomOut);
}

//************************************
// Method:    ShootWeapon
// FullName:  ATpsCharacter::ShootWeapon
// Access:    public 
// Returns:   void
// Qualifier: 
// Parameter: None
//************************************
void ATpsCharacter::ShootWeapon()
{
	// Fire only when zooming in
	if (ZoomingIn)
	{
		// ======= Step 1: Do single line tracing and fire weapon ========
		// Get the location and rotation of from camera's view
		FVector CamLocation;
		FRotator CamRotation;
		CameraComp->GetSocketWorldLocationAndRotation(USpringArmComponent::SocketName, CamLocation, CamRotation);

		// Get the end location of tracing line with a large distance
		FVector& StartLocation = CamLocation;
		FVector  EndLocation   = CamLocation + CamRotation.Vector() * 10000.f;

		// Better to specify the Collision Query Parameters as well
		// For a precise hit point; more costly but looks way more natural
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(CurrentWeapon);
		QueryParams.AddIgnoredActor(this);
		QueryParams.bTraceComplex = true;
		QueryParams.bReturnPhysicalMaterial = true; // Important!!!

		// Do line tracing by channel (ECC_Visibility: hit anything visible that blocks the line)
		// Note that the hit actor's collision should be enabled, especially the traced channel
		FHitResult HitResult;
		bool IsHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, COLLISION_WEAPON, QueryParams);
		if (CurrentWeapon)
		{
			CurrentWeapon->Fire(IsHit, HitResult, EndLocation);
		}


		// ======= Step 2: Add camera shake effect ========
		// Add camera shake effect
		APlayerController* PlayerController = Cast<APlayerController>(GetController());
		if (PlayerController && CamShakeClass)
		{
			PlayerController->ClientPlayCameraShake(CamShakeClass);
		}

		// Crucial: update the time of last shot for proper automatic fire
		LastFireTime = GetWorld()->TimeSeconds;
	}
}


