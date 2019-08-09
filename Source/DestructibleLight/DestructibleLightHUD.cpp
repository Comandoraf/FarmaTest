// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "DestructibleLightHUD.h"
#include "Engine/Canvas.h"
#include "Engine/Texture2D.h"
#include "TextureResource.h"
#include "CanvasItem.h"
#include "UObject/ConstructorHelpers.h"

ADestructibleLightHUD::ADestructibleLightHUD()
{
	// Set the crosshair texture
	static ConstructorHelpers::FObjectFinder<UTexture2D> CrosshairTexObj(TEXT("/Game/FirstPerson/Textures/FirstPersonCrosshair"));
	CrosshairTex = CrosshairTexObj.Object;
}


void ADestructibleLightHUD::DrawHUD()
{
	Super::DrawHUD();

	// Draw very simple crosshair

	// find center of the Canvas
	const FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);

	// offset by half the texture's dimensions so that the center of the texture aligns with the center of the Canvas
	const FVector2D CrosshairDrawPosition((Center.X),
		(Center.Y + 20.0f));

	// draw the crosshair
	FCanvasTileItem TileItem(CrosshairDrawPosition, CrosshairTex->Resource, FLinearColor::White);
	TileItem.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(TileItem);
	FString stringBuilder;
	stringBuilder.Append("LMB to shoot\n")
		.Append("RMB to change bullet color\n")
		.Append("Shoot lamp to change it's state\n")
		.Append("White bullet turn off\\on lamp\n")
		.Append("Others bullet changes lamp's colors\n")
		.Append("You can destroy columns (10 shoots needed)");
	DrawText(stringBuilder, FLinearColor::Red, 0.0f, Canvas->ClipY * 0.5f, (UFont*)0, 2.0f);
}
