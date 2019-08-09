// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "DestructibleLightProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "ConstructorHelpers.h" 
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "SyncLamp.h"

ADestructibleLightProjectile::ADestructibleLightProjectile()
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(15.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &ADestructibleLightProjectile::OnHit);		// set up a notification for when this component hits something blocking

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;
	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;

	ProjectileMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	ProjectileMeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMeshResource(TEXT("StaticMesh'/Game/FirstPerson/Meshes/FirstPersonProjectileMesh.FirstPersonProjectileMesh'"));
	if (StaticMeshResource.Succeeded())
	{
		ProjectileMeshComponent->SetStaticMesh(StaticMeshResource.Object);
		ProjectileMeshComponent->SetRelativeScale3D(FVector(0.1f, 0.1f, 0.1f));
		ProjectileMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	UMaterialInterface * Material = ProjectileMeshComponent->GetMaterial(0);
	UMaterialInstanceDynamic* matInstance = ProjectileMeshComponent->CreateDynamicMaterialInstance(0, Material);

	if (color == 4)
		actualColor = FLinearColor::White;
	else
		actualColor = FLinearColor(color == 1 ? 1.0f : 0.0f, //Red
			color == 2 ? 1.0f : 0.0f, //Green
			color == 3 ? 1.0f : 0.0f); //Blue

	if (matInstance != nullptr)
		matInstance->SetVectorParameterValue("DiffuseColor", actualColor);
	ProjectileMeshComponent->SetMaterial(0, matInstance);

}

void ADestructibleLightProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{
		if (Role == ROLE_Authority && OtherActor->Role == ROLE_Authority)
		{
			ASyncLamp* lamp = Cast<ASyncLamp>(OtherActor);
			if (lamp != nullptr)
			{
				if (this->GetActualColor() != FLinearColor::White)
					lamp->ServerSetLightColor(this->GetActualColor());
				else
					lamp->ServerSwitchLamp();
			}
			else
			{
				OtherActor->TakeDamage(10.0f, FDamageEvent(), nullptr, this);
			}
		}
		Destroy();
	}
}

int ADestructibleLightProjectile::color = 1;