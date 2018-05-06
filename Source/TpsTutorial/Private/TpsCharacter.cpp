// Fill out your copyright notice in the Description page of Project Settings.

#include "TpsCharacter.h"
#include <Components/InputComponent.h>
#include <Camera/CameraComponent.h>
#include <GameFramework/SpringArmComponent.h>
#include "../Public/TpsCharacter.h"


// Sets default values
ATpsCharacter::ATpsCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// When rotate the camera: don't rotate the character
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;

	// Auto posses player 0
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	// Create a Spring Arm Component
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(RootComponent); // Must-have
	SpringArmComp->bUsePawnControlRotation = true; // Must-have

	// Create a UCameraComponent for this player.
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);
}

// Called when the game starts or when spawned
void ATpsCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATpsCharacter::MoveForward(float axisValue)
{
	AddMovementInput(GetActorForwardVector() * axisValue);
}

void ATpsCharacter::MoveRight(float axisValue)
{
	AddMovementInput(GetActorRightVector() * axisValue);
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
}
