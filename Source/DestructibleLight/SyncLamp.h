// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SyncLamp.generated.h"

UCLASS()
class DESTRUCTIBLELIGHT_API ASyncLamp : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASyncLamp();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
		class UBoxComponent* CollisionComp;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	bool GetIsLampOn() const;
	void SetLightColor(FLinearColor color);
	void SwitchLamp();

private:

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* LampMesh;

	UPROPERTY(VisibleAnywhere)
	class ULightComponent* LightComponent;
public:
	UFUNCTION(Reliable, NetMulticast, WithValidation)
	void MulticastSwitchLamp();
	void MulticastSwitchLamp_Implementation();
	bool MulticastSwitchLamp_Validate() { return true; }

	UFUNCTION(Reliable, NetMulticast, WithValidation)
	void MulticastSetLightColor(FLinearColor color);
	void MulticastSetLightColor_Implementation(FLinearColor color);
	bool MulticastSetLightColor_Validate(FLinearColor color) { return true; }

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerSwitchLamp();
	void ServerSwitchLamp_Implementation();
	bool ServerSwitchLamp_Validate() { return true; }

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerSetLightColor(FLinearColor color);
	void ServerSetLightColor_Implementation(FLinearColor color);
	bool ServerSetLightColor_Validate(FLinearColor color) { return true; }

};
