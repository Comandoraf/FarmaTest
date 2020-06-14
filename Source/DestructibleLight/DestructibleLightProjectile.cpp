// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "DestructibleLightProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "ConstructorHelpers.h" 
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

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

}

void ADestructibleLightProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{
		if (Role == ROLE_Authority && OtherActor->Role == ROLE_Authority)
		{
			OtherActor->TakeDamage(10.0f, FDamageEvent(), nullptr, this);
		}
		Destroy();
	}
}