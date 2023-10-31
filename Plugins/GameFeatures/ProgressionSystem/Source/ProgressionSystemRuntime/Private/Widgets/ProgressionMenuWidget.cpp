﻿// Copyright (c) Yevhenii Selivanov


#include "Widgets/ProgressionMenuWidget.h"
//---
#include "Bomber.h"
#include "Controllers/MyPlayerController.h"
#include "UtilityLibraries/MyBlueprintFunctionLibrary.h"
//---
#include <string>

#include "ProgressionSystemComponent.h"
#include "ProgressionSystemDataAsset.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Image.h"
#include "GameFramework/MyGameStateBase.h"
#include "GameFramework/MyPlayerState.h"

void UProgressionMenuWidget::SetProgressionState(FText text)
{
	if (ProgressionState)
	{
		ProgressionState->SetText(text);
	}
}

void UProgressionMenuWidget::AddImagesToHorizontalBox(int AmountOfUnlockedPoints, int AmountOfLockedPoints)
{
	UE_LOG(LogTemp, Warning, TEXT("--- AddImagesToHorizontalBox running ---"));
	if (HorizontalBox)
	{
		UE_LOG(LogTemp, Warning, TEXT("--- HorizontalBox true ---"));
		// Loop to create and add images to the Horizontal Box for unlocked stars
		for (int i = 0; i < AmountOfUnlockedPoints; i++)
		{
			UImage* ImageWidget = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
			ImageWidget->SetBrushFromTexture(ProgressionSystemDataAssetInternal->GetUnlockedProgressionIcon());
			// Load and set the image texture here using ImagePath or other methods
			HorizontalBox->AddChildToHorizontalBox(ImageWidget);
			UE_LOG(LogTemp, Warning, TEXT("--- AddChildToHorizontalBox true ---"));

			// Create the UHorizontalBoxSlot and assign it to the ImageWidget
			UHorizontalBoxSlot* HorizontalBoxSlot = Cast<UHorizontalBoxSlot>(ImageWidget->Slot);
			if (HorizontalBoxSlot)
			{
				HorizontalBoxSlot->SetPadding(FMargin(20.0f, 10.0f, 20.0f, 10.0f));
				HorizontalBoxSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
				HorizontalBoxSlot->SetHorizontalAlignment(HAlign_Fill);
				HorizontalBoxSlot->SetVerticalAlignment(VAlign_Fill);
			}
		}

		// Loop to create and add images to the Horizontal Box for unlocked stars
		for (int i = 0; i < AmountOfLockedPoints; i++)
		{
			UImage* ImageWidget = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
			ImageWidget->SetBrushFromTexture(ProgressionSystemDataAssetInternal->GetLockedProgressionIcon());
			// Load and set the image texture here using ImagePath or other methods
			HorizontalBox->AddChildToHorizontalBox(ImageWidget);
			UE_LOG(LogTemp, Warning, TEXT("--- AddChildToHorizontalBox true ---"));
			// Create the UHorizontalBoxSlot and assign it to the ImageWidget
			UHorizontalBoxSlot* HorizontalBoxSlot = Cast<UHorizontalBoxSlot>(ImageWidget->Slot);
			if (HorizontalBoxSlot)
			{
				HorizontalBoxSlot->SetPadding(FMargin(20.0f, 10.0f, 20.0f, 10.0f));
				HorizontalBoxSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
				HorizontalBoxSlot->SetHorizontalAlignment(HAlign_Fill);
				HorizontalBoxSlot->SetVerticalAlignment(VAlign_Fill);
			}
		}
	}
}

void UProgressionMenuWidget::ClearImagesFromHorizontalBox()
{
	if (HorizontalBox)
	{
		HorizontalBox->ClearChildren();
	}
}

void UProgressionMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();
	// Hide this widget by default
	SetVisibility(ESlateVisibility::Collapsed);
}