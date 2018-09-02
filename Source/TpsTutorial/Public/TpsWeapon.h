// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TpsTutorial.h"
#include "TpsWeapon.generated.h"

// Forward declarations
class USkeletalMeshComponent;
class UStaticMeshComponent;
class UParticleSystem;
class UDamageType;
class ATpsProjectile;

// Enum to define the weapon typs
UENUM()
enum class EWeaponType: uint8
{
	EWT_Pistol,	EWT_Rifle 
};

UCLASS()
class TPSTUTORIAL_API ATpsWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATpsWeapon();

protected:
	// =============== Functions =====================

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Set the static mesh component for this weapon (in BP).
	UFUNCTION(BlueprintCallable, Category = Functions)
	void SetStaticMeshComponent(UStaticMeshComponent* StaticMeshComp);


	// =============== Variables =====================

	// The mesh component of this weapon
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComp;
	//USkeletalMeshComponent* MeshComp;

	// The Muzzle effect of this weapon when firing
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
	UParticleSystem* MuzzleEffect;
	
	// The line tracing effect when firing
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
	UParticleSystem* TracerEffect;

	// The default impact effect when it hits a target
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
	UParticleSystem* DefaultImpactEffect;

	// The impact effect when it hits flesh (e.g., humans)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
	UParticleSystem* FleshImpactEffect;

	// The damage type of this weapon
	UPROPERTY(EditDefaultsOnly, BLueprintReadOnly, Category = "Effects")
	TSubclassOf<UDamageType> DamageType;

	// The projectile class to spawn
	UPROPERTY(EditDefaultsOnly, Category = "Key Properties")
	TSubclassOf<ATpsProjectile> ProjectileClass;

	// The type of this weapon: pistol or rifle
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Key Properties")
	EWeaponType WeaponType;

	// Is this an automatic weapon. Default is false.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Key Properties")
	bool bAuto;

	// Base damage of this weapon. Default is 20
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Key Properties")
	float BaseDamage;

	// The number of fires per second. Default is 10
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Key Properties")
	float FireRatePerSecond;

	// The initial speed of bullet from this weapon
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Key Properties")
	float InitialBulletSpeed;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// The fire function of this weapon
	UFUNCTION(BlueprintCallable)
	virtual void Fire(const FVector& EndLocation);

	// Return if this weapon is automatic or not
	bool IsAutomatic() const;

	// Return the rate of fire per second for this weapon
	float GetFireRatePerSecond() const;
};
