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
#include "Animation/AnimMontage.h"

// Sets default values
ATpsCharacter::ATpsCharacter()
	// Initializer list with default values
	:bAiming(false)
	,ZoomSpeed(5.f)
	,ZoomInFov(30.f)
	,DefaultFov(70.f)
	,bDead(false)
	,CurrentWeaponID(EWeaponID::None)
	,AimRotation(5.f)
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
	GetCapsuleComponent()->ComponentTags.Add(TEXT("Player"));

	// Auto posses player 0
	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

// Called when the game starts or when spawned
void ATpsCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Spawn weapons
	SpawnWeapon();

	// Create crosshair widgets (implemented in BP)
	CreateCrosshairWidgetEvent();
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
	//PlayerInputComponent->BindAxis("MoveForward", this, &ATpsCharacter::MoveForward);
	//PlayerInputComponent->BindAxis("MoveRight", this, &ATpsCharacter::MoveRight);

	// Bind camera input
	PlayerInputComponent->BindAxis("Lookup", this, &ATpsCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn", this, &ATpsCharacter::AddControllerYawInput);

	// Bind actions(NOTE: cannot directly use Crouch() and UnCrouch() function here. Why??)
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ATpsCharacter::StartShoot);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ATpsCharacter::EndShoot);
	PlayerInputComponent->BindAction("Zoom", IE_Pressed, this, &ATpsCharacter::ZoomIn);
	PlayerInputComponent->BindAction("Zoom", IE_Released, this, &ATpsCharacter::ZoomOut);
	PlayerInputComponent->BindAction("Swap", IE_Pressed, this, &ATpsCharacter::SwapWeapon);
}

// Set timer and start shooting
void ATpsCharacter::StartShoot()
{
	// Check if the weapon is automatic and get its fire rate
	bool bLoop = EquippedWeapons[CurrentWeaponID]->IsAutomatic();
	float FirePeriod = 1.f / (EquippedWeapons[CurrentWeaponID]->GetFireRatePerSecond());

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
	GetCharacterMovement()->MaxWalkSpeed *= 0.35f;
	GetMesh()->AddLocalRotation(FRotator(0.f, AimRotation, 0.f));
	EquippedWeapons[CurrentWeaponID]->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("WeaponSocket_Rifle_Aiming"));

	// Use timeline to zoom in camera in BP
	ZoomInCamera();
}


void ATpsCharacter::ZoomOut()
{
	// Restore all default settings for player
	bAiming = false;
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->MaxWalkSpeed *= (1.f / 0.35f);
	GetMesh()->AddLocalRotation(FRotator(0.f, -AimRotation, 0.f));
	EquippedWeapons[CurrentWeaponID]->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("WeaponSocket_Idle"));

	// Use timeline to zoom out camera in BP
	ZoomOutCamera();
}


void ATpsCharacter::SwapWeapon()
{
	if (EquippedWeapons.Num() < 2) return;

	// Play weapon swapping montage
	if (WeaponSwapMontage)
	{
		PlayAnimMontage(WeaponSwapMontage);
	}
}


void ATpsCharacter::SwapWeaponAction()
{
	// Attach the equipped weapon to the right back
	EquippedWeapons[CurrentWeaponID]->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("Weapon_Holster_Right"));

	// Equip the back weapon to the hand
	EquippedWeapons[1 - CurrentWeaponID]->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("WeaponSocket_Idle"));

	// Update current weapon index
	CurrentWeaponID = static_cast<EWeaponID>(1 - CurrentWeaponID);
}


void ATpsCharacter::Fire()
{
	// Fire only when zooming in
	if (!bAiming || CurrentWeaponID == EWeaponID::None)
		return;

	// Do single line tracing by Weapon channel and fire weapon ========
	FHitResult HitResult;
	LineTraceFromCameraByChannel(HitResult, ECollisionChannel::COLLISION_WEAPON);
	EquippedWeapons[CurrentWeaponID]->Fire(HitResult);

	// Crucial: update the time of last shot for proper automatic fire
	LastFireTime = GetWorld()->TimeSeconds;

	// Play the firing animation
	if (FireWeaponMontage)	PlayAnimMontage(FireWeaponMontage);
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
bool ATpsCharacter::LineTraceFromCameraByChannel(FHitResult& HitResult, ECollisionChannel TraceChannel)
{
	// Get the location and rotation of from camera's view
	FVector CamLocation;
	FRotator CamRotation;
	CameraComp->GetSocketWorldLocationAndRotation(USpringArmComponent::SocketName, CamLocation, CamRotation);

	// Get the end location of tracing line with a large distance
	FVector EndLocation = CamLocation + CamRotation.Vector() * 5000000.f;

	// Better to specify the Collision Query Parameters as well
	// For a precise hit point; more costly but looks way more natural
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(EquippedWeapons[CurrentWeaponID]);
	QueryParams.AddIgnoredActor(this);
	QueryParams.bTraceComplex = true;
	QueryParams.bReturnPhysicalMaterial = true; // Important!!!

	// Do line tracing by channel (ECC_Visibility: hit anything visible that blocks the line
	// Note that the hit actor's collision should be enabled, especially the traced channel
	return GetWorld()->LineTraceSingleByChannel(HitResult, CamLocation, EndLocation, TraceChannel, QueryParams);
}

// Spawn the weapon at the right socket
void ATpsCharacter::SpawnWeapon()
{
	// If no weapon to spawn, return
	if (!LeftWeaponClass && !RightWeaponClass)
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
	for (uint8 i = 0; i < 1; i++)
	{
		// Get which class to spawn and socket name
		TSubclassOf<ATpsWeapon>& ClassToSpawn = (i == 0) ? LeftWeaponClass : RightWeaponClass;
		FName SocketName = (i == 0) ? TEXT("WeaponSocket_Idle") : TEXT("Weapon_Holster_Right");

		if (!ClassToSpawn) continue;

		// Spawn weapons and attach to the sockets
		ATpsWeapon* SpawnedWeapon = Cast<ATpsWeapon>(GetWorld()->SpawnActor(ClassToSpawn, &transform, params));
		if (SpawnedWeapon && GetMesh()->GetSocketByName(SocketName))
		{
			// Location Rule, Rotation Rule and Scale Rule all set to Snap to target
			// And Weld Simulated Bodies set to true
			SpawnedWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), SocketName);
			EquippedWeapons.Add(SpawnedWeapon);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("No socket named WeaponSocket!!! Please create one."));
		}
	}

	// Set the first spawned weapon as equipped
	CurrentWeaponID = EWeaponID::LeftWeapon;
}

