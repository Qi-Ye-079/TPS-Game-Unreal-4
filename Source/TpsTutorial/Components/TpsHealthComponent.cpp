// Fill out your copyright notice in the Description page of Project Settings.

#include "TpsHealthComponent.h"


// Sets default values for this component's properties
UTpsHealthComponent::UTpsHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;  // Not ticking

	// Set 2 health variables
	CurrentHealth = MaxHealth = 100.f;
}


// Called when the game starts
void UTpsHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// Get the owner
	AActor *Owner = GetOwner();
	if (Owner)
	{
		Owner->OnTakeAnyDamage.AddDynamic(this, &UTpsHealthComponent::UpdateHealthOnDamage);
	}
	
}


void UTpsHealthComponent::UpdateHealthOnDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	// If no damage, just return
	if (Damage <= 0.f)
		return;

	// Update health
	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.f, MaxHealth);

	// Log output
	UE_LOG(LogTemp, Log, TEXT("Health Updated: %s"), *FString::SanitizeFloat(CurrentHealth));

	// Implement the FOnHealthChangedSignature event by Broadcasting the delegate
	OnHealthChanged.Broadcast(this, CurrentHealth, Damage, DamageType, InstigatedBy, DamageCauser);
}

// Called every frame
void UTpsHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

