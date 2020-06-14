// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Camera/CameraComponent.h"
#include "Pickable.generated.h"

UCLASS()
class DESTRUCTIBLELIGHT_API APickable : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickable();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	/** Perform shoot on all instances*/

	void Pickup();

	UPROPERTY(VisibleAnywhere, Replicated)
	class UStaticMeshComponent* Mesh;

	/*void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;

	UPROPERTY(ReplicatedUsing = OnRep_alreadyDamaged)
		bool alreadyDamaged = false;*/

	/*UFUNCTION()
		void OnRep_alreadyDamaged();*/

	UPROPERTY(EditAnywhere)
		UTexture2D* texture;

	UPROPERTY(EditAnywhere)
		FString ObjectName;

};
