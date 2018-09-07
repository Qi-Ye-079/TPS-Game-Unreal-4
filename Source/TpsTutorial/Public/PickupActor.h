// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickupActor.generated.h"

// Forward declarations
class USphereComponent;
class UDecalComponent;
class USkeletalMeshComponent;
class APowerupActor;
class ATpsCharacter;

UENUM(BlueprintType)
enum class EPickupType: uint8
{
	None	UMETA(DisplayName = "None"), 
	Weapon	UMETA(DisplayName = "Weapon"), 
	Powerup	UMETA(DisplayName = "Powerup")
};

UENUM(BlueprintType)
enum class EWeapon: uint8
{
	None	UMETA(DisplayName = "None"), 
	W_G36	UMETA(DisplayName = "W_G36"), 
	W_M4	UMETA(DisplayName = "W_M4")
};

UENUM(BlueprintType)
enum class EPowerup: uint8
{
	None		UMETA(DisplayName = "None"), 
	P_Health	UMETA(DisplayName = "P_Health")
};

UCLASS()
class TPSTUTORIAL_API APickupActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickupActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly)
	ATpsCharacter* Player;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* SphereComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UDecalComponent* DecalComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USkeletalMeshComponent* MeshComp;

	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Properties")
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Properties")
	EPickupType PickupType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Properties")
	EWeapon WeaponName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Properties")
	EPowerup PowerupName;

public:	

	// Called when overlapping with other actors (overriding from Actor.h)
	// virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	
};
