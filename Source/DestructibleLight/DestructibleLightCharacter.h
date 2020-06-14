// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DestructibleLightCharacter.generated.h"

class UInputComponent;

UCLASS(config=Game)
class ADestructibleLightCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	class USkeletalMeshComponent* Mesh1P;

	/** Gun mesh: 1st person view (seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* FP_Gun;

	/** Location on gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USceneComponent* FP_MuzzleLocation;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCameraComponent;

public:
	ADestructibleLightCharacter();

protected:
	virtual void BeginPlay();

	virtual void Tick(float DeltaSeconds) override;

public:
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	FVector GunOffset;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		float PickupRange = 250.f;

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	TSubclassOf<class ADestructibleLightProjectile> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	class USoundBase* FireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UAnimMontage* FireAnimation;

protected:

	/**Array keeps what player has in inventory*/
	TArray<FString> inventory;

	/** Action Function */
	void OnAction();

	/** Fires a projectile. */
	void OnFire();

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	void ChangeColor();

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	class APickable* currentItem;

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

public:
	/** Returns Mesh1P subobject **/
	FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	/** Perform shoot on all instances*/
	UFUNCTION(Reliable, NetMulticast, WithValidation)
	void MulticastShoot(FRotator rotation);
	void MulticastShoot_Implementation(FRotator rotation);
	bool MulticastShoot_Validate(FRotator rotation) { return true; }

	/** Perform shoot on all instances*/
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerShoot(FRotator rotation);
	void ServerShoot_Implementation(FRotator rotation);
	bool ServerShoot_Validate(FRotator rotation) { return true; }

	/** Perform shoot on all instances*/
	UFUNCTION(Reliable, Server, WithValidation)
		void ServerAction();
	void ServerAction_Implementation();
	bool ServerAction_Validate() { return true; }

	/** Perform shoot on all instances*/
	UFUNCTION(Reliable, Client, WithValidation)
		void MulticastAction();
	void MulticastAction_Implementation();
	bool MulticastAction_Validate() { return true; }
	/*
	Performs raytrace to find closest looked-at UsableActor.
*/
	class APickable* GetUsableInView();
private:
	class AHUD* GetHUD();

	class ADestructibleLightHUD* CurrentHUD;
};

