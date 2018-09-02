// Fill out your copyright notice in the Description page of Project Settings.

#include "TpsCharacter.h"
#include "Components/TpsHealthComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "TimerManager.h"

// Sets default values
ATpsCharacter::ATpsCharacter()
	// Initializer list with default values
	:bAiming(false)
	,ZoomSpeed(10.f)
	,ZoomInFov(45.f)
	,DefaultFov(90.f)
	,ZoomHeight(75.f)
	,bDead(false)
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
	SpringArmComp->TargetArmLength = 300.f;                  // length of the spring arm
	SpringArmComp->SocketOffset = FVector(0.f, 60.f, 80.f);  // Offset at the end of spring arm(socket)
	SpringArmComp->bEnableCameraLag = true;
	SpringArmComp->CameraLagSpeed = 10.f;

	// Create a UCameraComponent for this player.
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);
	CameraComp->SetFieldOfView(DefaultFov);

	// Subscribe to the On Health Changed event in health component
	HealthComp = CreateDefaultSubobject<UTpsHealthComponent>(TEXT("HealthComp"));
	HealthComp->OnHealthChanged.AddDynamic(this, &ATpsCharacter::HandleHealthUpdate);

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

	// Spawn weapons
	SpawnWeapon();

	// Create crosshair and health indicator widgets (implemented in BP)
	CreateCrosshairWidgetEvent();
	CreateHealthIndicatorEvent();
}

// Called every frame
void ATpsCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Handle zoom in/out
	bAiming? ZoomAlpha += DeltaTime * ZoomSpeed : ZoomAlpha -= DeltaTime * ZoomSpeed;

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

	// Bind actions(NOTE: cannot directly use Crouch() and UnCrouch() function here. Why??)
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ATpsCharacter::StartShoot);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ATpsCharacter::EndShoot);
	PlayerInputComponent->BindAction("Zoom", IE_Pressed, this, &ATpsCharacter::ZoomIn);
	PlayerInputComponent->BindAction("Zoom", IE_Released, this, &ATpsCharacter::ZoomOut);
}

// Move forward or backward
void ATpsCharacter::MoveForward(float axisValue)
{
	// Get the direction of the controller
	const FRotator ControllerRotation = GetControlRotation();

	// Get the forward vector
	const FVector forward = FRotationMatrix(ControllerRotation).GetUnitAxis(EAxis::X);

	// Apply the forward vector to input
	AddMovementInput(forward, axisValue);
}

// Move left or right
void ATpsCharacter::MoveRight(float axisValue)
{
	// Get the direction to the right of the controller
	const FRotator ControllerRotation = GetControlRotation();

	// Get the right vector
	const FVector right = FRotationMatrix(ControllerRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(right, axisValue);
}

// Set timer and start shooting
void ATpsCharacter::StartShoot()
{
	// Check if the weapon is automatic and get its fire rate
	bool bLoop = CurrentWeapon->IsAutomatic();
	float FirePeriod = 1.f / (CurrentWeapon->GetFireRatePerSecond());

	// Determine the first delay of pressing the mouse
	float FirstDelay = FMath::Max(LastFireTime + FirePeriod - GetWorld()->TimeSeconds, 0.f);

	// Keeps shooting weapon at the desired rate
	GetWorldTimerManager().SetTimer(TimerHandle, this, &ATpsCharacter::Fire, FirePeriod, bLoop, FirstDelay);
}

// Clear timer to stop shooting
void ATpsCharacter::EndShoot()
{
	// Clear the timer handler
	GetWorldTimerManager().ClearTimer(TimerHandle);
}


void ATpsCharacter::ZoomIn()
{
	// Change all properties for aiming
	bAiming = true;
	bUseControllerRotationYaw = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->MaxWalkSpeed *= 0.5f;
	GetMesh()->AddLocalRotation(FRotator(0.f, 5.f, 0.f));
	CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("WeaponSocket_Rifle_Aiming"));
}


void ATpsCharacter::ZoomOut()
{
	// Restore all default settings for player
	bAiming = false;
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->MaxWalkSpeed *= 2.f;
	GetMesh()->AddLocalRotation(FRotator(0.f, -5.f, 0.f));
	CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("WeaponSocket_Idle"));
}


void ATpsCharacter::Fire()
{
	// Fire only when zooming in
	if (!bAiming || !CurrentWeapon)
		return;

	// Do single line tracing by Weapon channel and fire weapon ========
	ShootWeaponFromLineTraceChannel(ECollisionChannel::COLLISION_WEAPON);

	// Add camera shake effect
	/*
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController && CamShakeClass)
	{
		PlayerController->ClientPlayCameraShake(CamShakeClass);
	}

	// Crucial: update the time of last shot for proper automatic fire
	LastFireTime = GetWorld()->TimeSeconds;
	*/
}


void ATpsCharacter::HandleHealthUpdate(UTpsHealthComponent* OwningHealthComp, float CurrentHealth, float HealthDelta, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	UE_LOG(LogTemp, Log, TEXT("Health %s of %s"), *FString::SanitizeFloat(CurrentHealth), *GetName());

	// If current health is 0 and is alive: die!
	if (CurrentHealth > 0.f || bDead)
		return;

	// Set alive to false
	bDead = true;

	// Stop character's movement
	GetCharacterMovement()->StopMovementImmediately();

	// Disable all character's collisions
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Then play death animation. To be done in Animation BP...
}

/*
	Helper functions
 */
 // Helper function: Do single line trace by channel, determine end location, and fire weapon
void ATpsCharacter::ShootWeaponFromLineTraceChannel(ECollisionChannel TraceChannel)
{
	// Get the location and rotation of from camera's view
	FVector CamLocation;
	FRotator CamRotation;
	CameraComp->GetSocketWorldLocationAndRotation(USpringArmComponent::SocketName, CamLocation, CamRotation);

	// Get the end location of tracing line with a large distance
	FVector& StartLocation = CamLocation;
	FVector  EndLocation = CamLocation + CamRotation.Vector() * 10000.f;

	// Better to specify the Collision Query Parameters as well
	// For a precise hit point; more costly but looks way more natural
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(CurrentWeapon);
	QueryParams.AddIgnoredActor(this);
	QueryParams.bTraceComplex = true;
	QueryParams.bReturnPhysicalMaterial = true; // Important!!!

	// Do line tracing by channel (ECC_Visibility: hit anything visible that blocks the line
	// Note that the hit actor's collision should be enabled, especially the traced channel
	FHitResult HitResult;
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, TraceChannel, QueryParams);
	bHit ? CurrentWeapon->Fire(HitResult.ImpactPoint) : CurrentWeapon->Fire(EndLocation);
}

// Spawn the weapon at the right socket
void ATpsCharacter::SpawnWeapon()
{
	// If no weapon to spawn, return
	if (!WeaponClass)
	{
		UE_LOG(LogTemp, Error, TEXT("No Weapon class assigned. Pls do so in BP editor."));
		return;
	}

	// Get transformation of spawned actor (values not important because it will be attached 
	// to character's weapon socket later
	FTransform transform;

	// Get the spawn parameters
	FActorSpawnParameters params;
	params.Owner = this;
	params.Instigator = Instigator;

	// Spawn the actor (weapon)
	CurrentWeapon = Cast<ATpsWeapon>(GetWorld()->SpawnActor(WeaponClass, &transform, params));
	if (CurrentWeapon && GetMesh()->GetSocketByName(TEXT("WeaponSocket_Idle")))
	{
		// Location Rule, Rotation Rule and Scale Rule all set to Snap to target
		// And Weld Simulated Bodies set to true
		CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("WeaponSocket_Idle"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("No socket named WeaponSocket!!! Please create one."));
	}
}

