// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TpsHealthComponent.generated.h"

// Forward declarations
class UDamageType;
class AController;

// DEclare a multi-cast delegate event when health is changed
DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FOnHealthChangedSignature, UTpsHealthComponent*, OwningHealthComp, float, CurrentHealth, float, HealthDelta, const UDamageType*, DamageType, AController*, InstigatedBy, AActor*, DamageCauser);

UCLASS( ClassGroup=(CharacterHealth), meta=(BlueprintSpawnableComponent) )
class TPSTUTORIAL_API UTpsHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTpsHealthComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// Dynamic Delegate function to update health on damage
	UFUNCTION() // Must-have for dynamic delegate to work!!!
	void UpdateHealthOnDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

	UPROPERTY(BlueprintReadWrite, Category = "HealthComponent")
	float CurrentHealth;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HealthComponent")
	float MaxHealth;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Health (amount > 0) or damage (amount < 0)
	UFUNCTION(BlueprintCallable)
	void UpdateCurrentHealth(float DeltaHealth);

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnHealthChangedSignature OnHealthChanged;
	
};
