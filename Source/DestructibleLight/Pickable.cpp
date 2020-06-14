// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickable.h"
#include "GameFramework/Character.h"
#include "Engine/StaticMesh.h"
#include "ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "UnrealNetwork.h"

// Sets default values
APickable::APickable()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	//
	SetReplicates(true);
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SMResource(TEXT("StaticMesh'/Game/Weapon_Pack/Mesh/Weapons/Weapons_Kit/SM_Dagger_1.SM_Dagger_1'"));
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	if (SMResource.Succeeded())
		Mesh->SetStaticMesh(SMResource.Object);
	Mesh->SetupAttachment(RootComponent);
}

//void APickable::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
//{
//	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
//
//	DOREPLIFETIME(APickable, alreadyDamaged);
//}

//void APickable::OnRep_alreadyDamaged()
//{
//	OnAlreadyDamaged();
//}
//
//void APickable::OnAlreadyDamaged()
//{
//	if (alreadyDamaged)
//		this->Destroy();
//}
//
//float APickable::TakeDamage(float DamageAmount, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser)
//{
//	float dmg = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
//
//	alreadyDamaged = true;
//	OnAlreadyDamaged();
//
//	return dmg;
//}

// Called when the game starts or when spawned
void APickable::BeginPlay()
{
	Super::BeginPlay();

}

void APickable::Pickup()
{/*
	if (!alreadyDamaged)
	{
		alreadyDamaged = true;
		OnAlreadyDamaged();
	}*/
	this->Destroy();
}