// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KillableActor.generated.h"

UCLASS()
class DESTRUCTIBLELIGHT_API AKillableActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AKillableActor();
private:
	UPROPERTY(EditAnywhere)
		float MaxHealth = 100.0f;

	UPROPERTY(Replicated)
		float ActualHealth = 100.0f;

	//Visual effects
	UParticleSystem* SparksParticleSystem;

	UParticleSystem* FireParticleSystem;

	//component for explosion and sparks
	UPROPERTY()
	class UParticleSystemComponent* SparkParticleSystemComponent;

	class UParticleSystemComponent* FireParticleSystemComponent;

	bool bDuringDeath = false;

	UPROPERTY(VisibleAnywhere)
	class UDestructibleComponent* DestructibleMesh;

	class UMaterialInstanceDynamic* matInstance;

public:
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;

	void BeginPlay() override;

	UFUNCTION(Reliable, NetMulticast, WithValidation)
	void Kill(FRotator rotator);
	void Kill_Implementation(FRotator rotator);
	bool Kill_Validate(FRotator rotator) { return true; }

	float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController * EventInstigator, AActor * DamageCauser) override;

	//void PlayVisualEffects(FTransform transform);
	UFUNCTION(Reliable, NetMulticast, WithValidation)
	void PlayVisualEffects(FTransform transform);
	void PlayVisualEffects_Implementation(FTransform transform);
	bool PlayVisualEffects_Validate(FTransform transform) { return true; }
};
