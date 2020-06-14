// Fill out your copyright notice in the Description page of Project Settings.


#include "KillableActor.h"

#include "UnrealNetwork.h"
#include "Engine/Classes/Kismet/GameplayStatics.h"
#include "Engine/Classes/Particles/ParticleSystemComponent.h"
#include "ConstructorHelpers.h"
#include "DestructibleMesh.h"
#include "DestructibleComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

// Sets default values
AKillableActor::AKillableActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bCanBeDamaged = true;

	SetReplicates(true);
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	if (!alreadyDamaged)
	{
		DestructibleMesh = CreateDefaultSubobject<UDestructibleComponent>(TEXT("DesctructibleMesh"));
		DestructibleMesh->AttachTo(RootComponent);

		static ConstructorHelpers::FObjectFinder<UDestructibleMesh> StaticMeshResource(TEXT("DestructibleMesh'/Game/StarterContent/Architecture/Pillar_50x500_DM.Pillar_50x500_DM'"));
		static ConstructorHelpers::FObjectFinder<UDestructibleMesh> StaticMeshResourceDamaged(TEXT("DestructibleMesh'/Game/Meshes/StaticMesh_DM.StaticMesh_DM'"));
		if (StaticMeshResourceDamaged.Succeeded())
		{
			damagedmesh = StaticMeshResourceDamaged.Object;
		}
		if (StaticMeshResource.Succeeded())
		{
			DestructibleMesh->SetDestructibleMesh(StaticMeshResource.Object);
		}
		else
			UE_LOG(LogTemp, Warning, TEXT("Wrong"));
	}

	//Particles resources
	static ConstructorHelpers::FObjectFinder<UParticleSystem> FireParticleResource(TEXT("ParticleSystem'/Game/StarterContent/Particles/P_Fire.P_Fire'"));
	if (FireParticleResource.Succeeded()) FireParticleSystem = FireParticleResource.Object;
	static ConstructorHelpers::FObjectFinder<UParticleSystem> SparksParticleResource(TEXT("ParticleSystem'/Game/StarterContent/Particles/P_Sparks.P_Sparks'"));
	if (SparksParticleResource.Succeeded()) SparksParticleSystem = SparksParticleResource.Object;
	static ConstructorHelpers::FObjectFinder<USoundBase> ExplosionAudioResource(TEXT("SoundWave'/Game/FirstPerson/Audio/Explosion+3.Explosion+3'"));
	if (ExplosionAudioResource.Succeeded()) ExplosionAudio = ExplosionAudioResource.Object;

}

void AKillableActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AKillableActor, ActualHealth);
	DOREPLIFETIME(AKillableActor, alreadyDamaged);
}

void AKillableActor::OnHealthUpdate()
{
	float color = ActualHealth / 100.0f;

	if (matInstance != nullptr)
		matInstance->SetVectorParameterValue("Color", FLinearColor(color, color, color));
	DestructibleMesh->SetMaterial(0, matInstance);

	if (ActualHealth == 0.0f)
		Kill();
}

void AKillableActor::OnRep_CurrentHealth()
{
	if (!alreadyDamaged)
		OnHealthUpdate();
}

void AKillableActor::BeginPlay()
{

	if (DestructibleMesh != nullptr)
	{
		UMaterialInterface * Material = DestructibleMesh->GetMaterial(0);
		if (Material != nullptr)
			matInstance = DestructibleMesh->CreateDynamicMaterialInstance(0, Material);
	}

}
void AKillableActor::OnRep_alreadyDamaged()
{
	if (alreadyDamaged)
	{
		DestructibleMesh->SetDestructibleMesh(damagedmesh);
		DestructibleMesh->LargeChunkThreshold = 10000.0f;
		DestructibleMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		if (matInstance != nullptr)
			matInstance->SetVectorParameterValue("Color", FLinearColor(0.0f, 0.0f, 0.0f));
		DestructibleMesh->SetMaterial(0, matInstance);
		FireParticleSystemComponent = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), FireParticleSystem, GetActorTransform());
	}
}

void AKillableActor::Kill_Implementation()
{
	if (!bDuringDeath && !alreadyDamaged)
	{
		FVector Origin, Extent;
		GetActorBounds(true, Origin, Extent);
		bDuringDeath = true;
		DestructibleMesh->ApplyDamage(100.0f, Origin, FVector::ZeroVector, 0.0f);
		DestructibleMesh->LargeChunkThreshold = 10000.0f;
		UGameplayStatics::PlaySoundAtLocation(this, ExplosionAudio, GetActorTransform().GetLocation());
		alreadyDamaged = true;
		FireParticleSystemComponent = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), FireParticleSystem, GetActorTransform());
	}
}

float AKillableActor::TakeDamage(float DamageAmount, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser)
{
	float dmg = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (Role == ROLE_Authority)
	{
		ActualHealth = FMath::Max(0.0f, ActualHealth - dmg);
		if (ActualHealth > 0.0f)
			PlayVisualEffects(DamageCauser->GetTransform());
		OnHealthUpdate();

	}

	return dmg;
}

void AKillableActor::PlayVisualEffects_Implementation(FTransform transform)
{
	if (ActualHealth > 0.0f)
	{
		SparkParticleSystemComponent = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SparksParticleSystem, transform);
	}
}