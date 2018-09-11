// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyAI.generated.h"

class UTpsHealthComponent;
class UDamageType;
class AController;

UCLASS()
class TPSTUTORIAL_API AEnemyAI : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyAI();

protected:
	// ------------- Functions ------------------

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called when health is updated
	UFUNCTION()
	void HandleHealthUpdate(UTpsHealthComponent* OwningHealthComp, float CurrentHealth, float HealthDelta, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

	// ------------- Variables ------------------
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UTpsHealthComponent* HealthComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Properties")
	TArray<AActor*> PatrolWayPoints;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	
	
};
