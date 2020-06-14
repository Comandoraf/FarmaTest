// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "DestructibleLightCharacter.h"
#include "DestructibleLightProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "Kismet/GameplayStatics.h"
#include "DestructibleLightHUD.h"
#include "ConstructorHelpers.h"
#include "Animation/AnimBlueprint.h"
#include "Engine/World.h"
#include "Pickable.h"
#include "Engine/LocalPlayer.h"
#include "Engine/Engine.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// ADestructibleLightCharacter

ADestructibleLightCharacter::ADestructibleLightCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->RelativeLocation = FVector(-39.56f, 1.75f, 64.f); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->RelativeRotation = FRotator(1.9f, -19.19f, 5.2f);
	Mesh1P->RelativeLocation = FVector(-0.5f, -4.4f, -155.7f);

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	// FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
	FP_Gun->SetupAttachment(RootComponent);

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 0.0f, 10.0f);

	//Load resources

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> BodySkeletalMeshResource(TEXT("SkeletalMesh'/Game/FirstPerson/Character/Mesh/SK_Mannequin_Arms.SK_Mannequin_Arms'"));
	if (BodySkeletalMeshResource.Succeeded())
	{
		Mesh1P->SetSkeletalMesh(BodySkeletalMeshResource.Object);
		Mesh1P->SetOnlyOwnerSee(false);
	}


	static ConstructorHelpers::FObjectFinder<UAnimBlueprint> BodyAnimResource(TEXT("AnimBlueprint'/Game/FirstPerson/Animations/FirstPerson_AnimBP.FirstPerson_AnimBP'"));
	if (BodyAnimResource.Succeeded())
		Mesh1P->SetAnimInstanceClass(BodyAnimResource.Object->GeneratedClass);

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> GunMeshResource(TEXT("SkeletalMesh'/Game/FirstPerson/FPWeapon/Mesh/SK_FPGun.SK_FPGun'"));
	if (GunMeshResource.Succeeded())
	{
		FP_Gun->SetSkeletalMesh(GunMeshResource.Object);
		FP_Gun->SetOnlyOwnerSee(false);
	}

	static ConstructorHelpers::FObjectFinder<USoundBase> FireSoundResource(TEXT("SoundWave'/Game/FirstPerson/Audio/FirstPersonTemplateWeaponFire02.FirstPersonTemplateWeaponFire02'"));
	if (FireSoundResource.Succeeded())
		FireSound = FireSoundResource.Object;

	static ConstructorHelpers::FObjectFinder<UAnimMontage> FireAnimResource(TEXT("AnimMontage'/Game/FirstPerson/Animations/FirstPersonFire_Montage.FirstPersonFire_Montage'"));
	if (FireAnimResource.Succeeded())
		FireAnimation = FireAnimResource.Object;

	ProjectileClass = ADestructibleLightProjectile::StaticClass();

}

void ADestructibleLightCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));

	CurrentHUD = Cast<ADestructibleLightHUD>(GetHUD());
}

APickable* ADestructibleLightCharacter::GetPickableInView()
{
	if (Controller == nullptr)
		return nullptr;
	FVector cameraLocation;
	FRotator cameraRotation;

	Controller->GetPlayerViewPoint(cameraLocation, cameraRotation);
	FVector start = cameraLocation;
	FVector forwardVector = cameraRotation.Vector();
	FVector end = start + (forwardVector * PickupRange);

	FHitResult Hit;
	FComponentQueryParams DefaultComponentQueryParams;
	FCollisionResponseParams DefaultResponseParam;

	if (GetWorld()->LineTraceSingleByChannel(Hit, start, end, ECC_Visibility, DefaultComponentQueryParams, DefaultResponseParam))
	{

		if (Hit.GetActor()->GetClass()->IsChildOf(APickable::StaticClass()))
		{
			return Cast<APickable>(Hit.GetActor());
		}
	}
	return nullptr;
}

void ADestructibleLightCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	currentItem = GetPickableInView();
	if (currentItem)
	{
		if (CurrentHUD)
			CurrentHUD->PickableInRange(currentItem->ObjectName);
	}
	else
	{
		if (CurrentHUD)
			CurrentHUD->PickableInRange(FString(""));
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void ADestructibleLightCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind action event
	PlayerInputComponent->BindAction("Action", IE_Pressed, this, &ADestructibleLightCharacter::OnAction);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ADestructibleLightCharacter::OnFire);
	// Bind change color event
	PlayerInputComponent->BindAction("ChangeColor", IE_Pressed, this, &ADestructibleLightCharacter::ChangeColor);
	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &ADestructibleLightCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ADestructibleLightCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ADestructibleLightCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ADestructibleLightCharacter::LookUpAtRate);
}
void ADestructibleLightCharacter::ServerAction_Implementation()
{
	currentItem = GetPickableInView();
	if (currentItem)
	{
		currentItem->Destroy();
		ClientAction();
	}
	
}
void ADestructibleLightCharacter::OnAction()
{
	ServerAction();
}
void ADestructibleLightCharacter::ClientAction_Implementation()
{
	if (currentItem)
	{
		inventory.Add(currentItem->ObjectName);
		CurrentHUD->PickUpItem(currentItem->texture);
	}
}

void ADestructibleLightCharacter::MulticastShoot_Implementation(FRotator rotation)
{
	// fire a projectile
	if (ProjectileClass != nullptr)
	{
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			FRotator SpawnRotation;
			if (Role == ROLE_SimulatedProxy)
				SpawnRotation = rotation;
			else
				SpawnRotation = GetControlRotation();
			// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
			const FVector SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);

			//Set Spawn Collision Handling Override
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

			// spawn the projectile at the muzzle
			World->SpawnActor<ADestructibleLightProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
		}
	}

	// play the sound if specified
	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	// try and play a firing animation if specified
	if (FireAnimation != nullptr)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
}

void ADestructibleLightCharacter::ServerShoot_Implementation(FRotator rotation)
{
	MulticastShoot(rotation);
}

AHUD * ADestructibleLightCharacter::GetHUD()
{
	APlayerController* PC = GetWorld()->GetFirstLocalPlayerFromController()->GetPlayerController(GetWorld());
	if (!PC)
	{
		return nullptr;
	}

	return PC->GetHUD();
}

void ADestructibleLightCharacter::OnFire()
{
	if (Role == ROLE_Authority)
	{
		MulticastShoot(GetControlRotation());
	}
	else
	{
		ServerShoot(GetControlRotation());
	}
}

void ADestructibleLightCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void ADestructibleLightCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void ADestructibleLightCharacter::ChangeColor()
{
	static int num = 1;
	ADestructibleLightProjectile::color = num;
	num++;
	if (num > 4) num = 1;
}

void ADestructibleLightCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ADestructibleLightCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}