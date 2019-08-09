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

	DestructibleMesh = CreateDefaultSubobject<UDestructibleComponent>(TEXT("DesctructibleMesh"));
	DestructibleMesh->AttachTo(RootComponent);
	static ConstructorHelpers::FObjectFinder<UDestructibleMesh> StaticMeshResource(TEXT("DestructibleMesh'/Game/StarterContent/Architecture/Pillar_50x500_DM.Pillar_50x500_DM'"));
	if (StaticMeshResource.Succeeded())
		DestructibleMesh->SetDestructibleMesh(StaticMeshResource.Object);
	else
		UE_LOG(LogTemp, Warning, TEXT("Wrong"));

	//Particles resources
	static ConstructorHelpers::FObjectFinder<UParticleSystem> FireParticleResource(TEXT("ParticleSystem'/Game/StarterContent/Particles/P_Fire.P_Fire'"));
	if (FireParticleResource.Succeeded()) FireParticleSystem = FireParticleResource.Object;
	static ConstructorHelpers::FObjectFinder<UParticleSystem> SparksParticleResource(TEXT("ParticleSystem'/Game/StarterContent/Particles/P_Sparks.P_Sparks'"));
	if (SparksParticleResource.Succeeded()) SparksParticleSystem = SparksParticleResource.Object;
}

void AKillableActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AKillableActor, ActualHealth);
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

void AKillableActor::Kill_Implementation(FRotator rotator)
{
	if (!bDuringDeath)
	{
		FVector Origin, Extent;
		GetActorBounds(true, Origin, Extent);
		bDuringDeath = true;
		DestructibleMesh->ApplyDamage(100.0f, Origin, rotator.Vector(), 10000.0f);
		DestructibleMesh->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
		FireParticleSystemComponent = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), FireParticleSystem, GetActorTransform());
	}
}

float AKillableActor::TakeDamage(float DamageAmount, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser)
{
	float dmg = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	
	if (Role == ROLE_Authority)
	{
		ActualHealth = FMath::Max(0.0f, ActualHealth - dmg);
		
		if (ActualHealth == 0.0f)
			Kill(DamageCauser->GetActorRotation());
		else
			PlayVisualEffects(DamageCauser->GetTransform());
	}

	return dmg;
}

void AKillableActor::PlayVisualEffects_Implementation(FTransform transform)
{
	if (ActualHealth > 0.0f)
	{
		float color = ActualHealth / 100.0f;

		if (matInstance != nullptr)
			matInstance->SetVectorParameterValue("Color", FLinearColor(color, color, color));
		DestructibleMesh->SetMaterial(0, matInstance);
		//Thanks to UPROPERTY there's no memory leak, and particles acts randomly with commented code (sometimes particles don't start)
		SparkParticleSystemComponent = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SparksParticleSystem, transform);
		/*if (!OneTimePlayParticle)
		{
			OneTimePlayParticle = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SparksParticleSystem, transform);
		}
		else
		{
			OneTimePlayParticle->SetWorldTransform(transform);
			OneTimePlayParticle->Activate(true);
		}*/
	}
}