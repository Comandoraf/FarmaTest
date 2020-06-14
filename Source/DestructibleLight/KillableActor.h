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
	UPROPERTY(EditDefaultsOnly, Category = "Health")
		float MaxHealth = 100.0f;

	UPROPERTY(ReplicatedUsing=OnRep_CurrentHealth)
		float ActualHealth = 100.0f;

	UPROPERTY(ReplicatedUsing=OnRep_alreadyDamaged)
		bool alreadyDamaged = false;

	UFUNCTION()
		void OnRep_alreadyDamaged();
	UFUNCTION()
		void OnRep_CurrentHealth();

	void OnHealthUpdate();

	//Visual effects
	UParticleSystem* SparksParticleSystem;

	UParticleSystem* FireParticleSystem;

	class USoundBase* ExplosionAudio;
	//component for explosion and sparks
	UPROPERTY()
	class UParticleSystemComponent* SparkParticleSystemComponent;

	class UParticleSystemComponent* FireParticleSystemComponent;

	bool bDuringDeath = false;

	UPROPERTY(VisibleAnywhere, Replicated)
	class UDestructibleComponent* DestructibleMesh;

	class UDestructibleMesh* damagedmesh;

	class UMaterialInstanceDynamic* matInstance;


public:
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;

	void BeginPlay() override;

	UFUNCTION(Reliable, NetMulticast, WithValidation)
	void Kill();
	void Kill_Implementation();
	bool Kill_Validate() { return true; }

	float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController * EventInstigator, AActor * DamageCauser) override;

	//void PlayVisualEffects(FTransform transform);
	UFUNCTION(Reliable, NetMulticast, WithValidation)
	void PlayVisualEffects(FTransform transform);
	void PlayVisualEffects_Implementation(FTransform transform);
	bool PlayVisualEffects_Validate(FTransform transform) { return true; }
};
