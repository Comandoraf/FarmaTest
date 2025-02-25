// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "DestructibleLightHUD.generated.h"

UCLASS()
class ADestructibleLightHUD : public AHUD
{
	GENERATED_BODY()

public:
	ADestructibleLightHUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

	void PickableInRange(FString name);
	void PickUpItem(UTexture2D* texture);
private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;
	bool pickableInRange = false;
	FString pickableName = "";
	TArray< UTexture2D*> inventory;

};

