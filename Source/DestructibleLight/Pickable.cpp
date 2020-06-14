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
	SetReplicates(true);
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	Mesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void APickable::BeginPlay()
{
	Super::BeginPlay();
}

void APickable::Pickup()
{
	this->Destroy();
}