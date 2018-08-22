// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "TpsTrackerBot.generated.h"

// Forward declarations
class UStaticMeshComponent;
class UTpsHealthComponent;
class UDamageType;
class AController;
class UParticleSystem;
class USphereComponent;
class USoundCue;
class UAudioComponent;

UCLASS()
class TPSTUTORIAL_API ATpsTrackerBot : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ATpsTrackerBot();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Get next path point
	UFUNCTION(BlueprintCallable)
	FVector GetNextPathPoint();

	// Self destruct when hp is 0
	void Explode();

	// Damage itself
	void DamageSelf();

	UFUNCTION()
	void HandleOnTakeDamage(UTpsHealthComponent *OwningHealthComp, float CurrentHealth, float HealthDelta, 
			const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	UStaticMeshComponent *StaticMeshComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	UTpsHealthComponent *HealthComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	USphereComponent* SphereComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	UAudioComponent* AudioComp;


	// Keep track of next path point
	FVector NextPathPoint;

	// The magnitude of Force to apply to this tracker bot
	UPROPERTY(EditDefaultsOnly, Category = "Properties")
	float ForceMag;

	// Whether or not the Force is taken as a change of acceleration instead of force.
	// If true, mass will no effect.
	UPROPERTY(EditDefaultsOnly, Category = "Properties")
	bool bAccelChangeInVelocity;

	// The minimal required distance between this and target.
	// When the distance is smaller than this value, this will get to next path point.
	UPROPERTY(EditDefaultsOnly, Category = "Properties")
	float RequiredDistanceToTarget;

	// The material instance to pulse on taking damage
	UMaterialInstanceDynamic* MatInstance;
	
	// The explosion effect
	UPROPERTY(EditDefaultsOnly, Category = "Properties")
	UParticleSystem* ExplosionEffect;

	// The explosion base when hp is 0
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
	float ExplodeBaseDamage;

	// The self damage/interval when it's near player
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
	float SelfDamage;

	// The interval between self damages
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
	float SelfDamageInterval;

	// The explosion radius
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
	float ExplodeRadius;

	// Timer handle for self-damaging when near player
	FTimerHandle TimerHandle_SelfDamage;

	// The sound effect when exploding
	UPROPERTY(EditDefaultsOnly, Category = "Properties")
	USoundCue* ExplodeSoundEffect;

	// The sound effect when self damaging starts
	UPROPERTY(EditDefaultsOnly, Category = "Properties")
	USoundCue* SelfDamageWarningSoundEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Properties")
	float SoundInRangeA;

	UPROPERTY(EditDefaultsOnly, Category = "Properties")
	float SoundInRangeB;

	UPROPERTY(EditDefaultsOnly, Category = "Properties")
	float SoundOutRangeA;

	UPROPERTY(EditDefaultsOnly, Category = "Properties")
	float SoundOutRangeB;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Override overlapping event with other actor
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
};
