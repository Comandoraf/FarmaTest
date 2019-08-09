// Fill out your copyright notice in the Description page of Project Settings.


#include "SyncLamp.h"
#include "Components/PointLightComponent.h"
#include "ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "DestructibleLightProjectile.h"

// Sets default values
ASyncLamp::ASyncLamp()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetReplicates(true);
	LightComponent = CreateDefaultSubobject<UPointLightComponent>(TEXT("LightComponent"));
	LightComponent->SetVisibility(true);
	LightComponent->SetIntensity(25000.0f);
	RootComponent = Cast<USceneComponent>(LightComponent);

	LampMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	LampMesh->AttachTo(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshResource(TEXT("StaticMesh'/Game/StarterContent/Props/SM_Lamp_Ceiling.SM_Lamp_Ceiling'"));
	if (MeshResource.Succeeded())
	{
		LampMesh->SetStaticMesh(MeshResource.Object);
		LampMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 130.0f));
	}

	CollisionComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	CollisionComp->InitBoxExtent(FVector(30.0f, 30.0f, 60.0f));
	CollisionComp->AttachTo(RootComponent);
	CollisionComp->SetRelativeLocation(FVector(0.0f, 0.0f, 60.0f));
	CollisionComp->BodyInstance.SetCollisionProfileName("BlockAllDynamic");
}

// Called when the game starts or when spawned
void ASyncLamp::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ASyncLamp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool ASyncLamp::GetIsLampOn() const
{
	return LightComponent->IsVisible();
}

void ASyncLamp::SetLightColor(FLinearColor color)
{
	LightComponent->SetLightColor(color);
}

void ASyncLamp::SwitchLamp()
{
	LightComponent->ToggleVisibility();
}

void ASyncLamp::MulticastSwitchLamp_Implementation()
{
	SwitchLamp();
}

void ASyncLamp::MulticastSetLightColor_Implementation(FLinearColor color)
{
	SetLightColor(color);
}

void ASyncLamp::ServerSwitchLamp_Implementation()
{
	MulticastSwitchLamp();
}

void ASyncLamp::ServerSetLightColor_Implementation(FLinearColor color)
{
	MulticastSetLightColor(color);
}
