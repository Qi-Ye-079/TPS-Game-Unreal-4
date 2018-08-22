// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "TpsTrackerBot.generated.h"

class UStaticMeshComponent;
class UTpsHealthComponent;
class UDamageType;
class AController;
class UParticleSystem;

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

	UFUNCTION()
	void HandleOnTakeDamage(UTpsHealthComponent *OwningHealthComp, float CurrentHealth, float HealthDelta, 
			const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

	UPROPERTY(VisibleDefaultsOnly)
	UStaticMeshComponent *StaticMeshComp;

	UPROPERTY(VisibleDefaultsOnly)
	UTpsHealthComponent *HealthComp;

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

	// The explosion radius
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
	float ExplodeRadius;



public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


};
