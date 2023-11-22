﻿// Copyright (c) Yevhenii Selivanov

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ProgressionMenuWidget.generated.h"
enum class ECurrentGameState : uint8;


/**
 * 
 */
UCLASS()
class PROGRESSIONSYSTEMRUNTIME_API UProgressionMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	public:

	// Horizontal Box widget for storing stars
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "C++", meta = (BlueprintProtected, BindWidget))
	TObjectPtr<class UHorizontalBox> HorizontalBox = nullptr;

	// Function to add images to the Horizontal Box
	UFUNCTION(BlueprintCallable)
	void AddImagesToHorizontalBox(int AmountOfUnlockedPoints, int AmountOfLockedPoints);

	// Function to add images to the Horizontal Box
	UFUNCTION(BlueprintCallable)
	void ClearImagesFromHorizontalBox();
	
	/*********************************************************************************************
	 * Protected functions
	 ********************************************************************************************* */
protected:
	/** Called after the underlying slate widget is constructed.
	 * May be called multiple times due to adding and removing from the hierarchy. */
	virtual void NativeConstruct() override;

	/** Progression System data asset */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BlueprintProtected, DisplayName = "Progression System Data Asset"))
	TObjectPtr<class UProgressionSystemDataAsset> ProgressionSystemDataAssetInternal = nullptr;

	/** Amount of Stars unlocked on a level */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BlueprintProtected, DisplayName = "Amount of Unlocked points debug"))
	int AmountOfStarsUnlockedDebug = 0;

	/** Amount of Stars locked on a level */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BlueprintProtected, DisplayName = "Amount of locked points debug"))
	int AmountOfStarsLockedDebug = 0;
};
