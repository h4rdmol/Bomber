﻿// Copyright (c) Yevhenii Selivanov

#pragma once

#include "ProgressionSystemDataAsset.h"
#include "Components/ActorComponent.h"
#include "Blueprint/UserWidget.h"
#include "Structures/PlayerTag.h"
//---
#include "ProgressionSystemComponent.generated.h"

/**
 * Implements the core logic on project about Progression System.
 */

USTRUCT(BlueprintType)
struct FSavedProgression
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Current Progression")
	FName ProgressionRowName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Current Progression")
	int Points;
};
UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROGRESSIONSYSTEMRUNTIME_API UProgressionSystemComponent final : public UActorComponent
{
	GENERATED_BODY()

public:
	/** Sets default values for this component's properties. */
	UProgressionSystemComponent();

	/** Returns the Progression System data asset. */
	UFUNCTION(BlueprintPure, Category = "C++")
	static const UProgressionSystemDataAsset* GetProgressionSystemDataAsset() {return &UProgressionSystemDataAsset::Get(); }
	
	/** Returns current saved progression. */
	UFUNCTION(BlueprintCallable, Category="C++")
	FORCEINLINE FSavedProgression GetSavedProgression() const { return SavedProgressionInternal; }

	/** Returns the endgame reward. */
	UFUNCTION(BlueprintCallable, Category="C++")
	FORCEINLINE int GetCurrenTotalScore() const { return SavedProgressionInternal.Points; }

	/** Save the progression depends on EEndGameState. */
	UFUNCTION(BlueprintCallable, Category="C++")
	void SavePoints(ELevelType Map, FPlayerTag Character, EEndGameState EndGameState);

	/** Checks and updates the progression row name if points to unlock for the next progression reached. */
	UFUNCTION(BlueprintCallable, Category="C++")
	void UpdateProgressionRowName();

	/** Returns the endgame reward. */
	UFUNCTION(BlueprintCallable, Category="C++")
	int GetProgressionReward(ELevelType Map, FPlayerTag Character, EEndGameState EndGameState);
	
protected:
	
	/** Called when the game starts. */
	virtual void BeginPlay() override;

	/** Returns a current progression row name */
	UFUNCTION(BlueprintPure, Category="C++")
	FName GetProgressionRow(ELevelType Map, FPlayerTag Character);

	/** Returns a current progression row name */
	UFUNCTION(BlueprintPure, Category="C++")
	FProgressionRowData GetProgressionRowData(ELevelType Map, FPlayerTag Character);
	
	/** Returns a current progression row name */
	UFUNCTION(BlueprintPure, Category="C++")
	int GetMaxAchievableLevelPoints(ELevelType Map, FPlayerTag Character);

	/** Progression System data asset */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BlueprintProtected, DisplayName = "Progression System Data Asset"))
	TObjectPtr<UProgressionSystemDataAsset> ProgressionSystemDataAssetInternal;

	/** Created Main Menu widget. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Transient, Category = "C++", meta = (BlueprintProtected, DisplayName = "Main Menu Widget"))
	TObjectPtr<class UProgressionMenuWidget> ProgressionMenuWidgetInternal = nullptr;

	/** Created Save points widget. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Transient, Category = "C++", meta = (BlueprintProtected, DisplayName = "Main Menu Widget"))
	TObjectPtr<class UProgressionSaveWidget> ProgressionSaveWidgetInternal = nullptr;

	/** The current Saved Progression of a player. */
	UPROPERTY(EditDefaultsOnly, meta = (DisplayName = "Saved Progression", ShowOnlyInnerProperties))
	FSavedProgression SavedProgressionInternal;

	/** The current selected player */
	UPROPERTY(EditDefaultsOnly, meta = (DisplayName = "Currnet Player Tag", ShowOnlyInnerProperties))
	FPlayerTag CurrentPlayerTagInternal;

	/** The Progression Data Table that is responsible for progression configuration. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BlueprintProtected, DisplayName = "Progression Data Table", ShowOnlyInnerProperties))
	TObjectPtr<UDataTable> ProgressionDataTableInternal = nullptr;
	
	/** Called when the current game state was changed. */
	UFUNCTION(BlueprintCallable, Category = "C++", meta = (BlueprintProtected))
	void OnGameStateChanged(ECurrentGameState CurrentGameState);

	/** Called when the end game state was changed. */
	UFUNCTION(BlueprintCallable, Category = "C++", meta = (BlueprintProtected))
	void OnEndGameStateChanged(EEndGameState EndGameState);
	
	/** Is called to prepare the widget for Menu game state. */
	UFUNCTION(BlueprintCallable, Category = "C++", meta = (BlueprintProtected))
	void HandleGameState(class AMyGameStateBase* MyGameState);

	/** Is called to prepare the widget for handling end game state. */
	UFUNCTION(BlueprintCallable, Category = "C++", meta = (BlueprintProtected))
	void HandleEndGameState(class AMyPlayerState* MyPlayerState);

	/** Is called when a player has been changed */
	UFUNCTION(BlueprintCallable, Category= "C++", meta = (BlueprintProtected))
	void OnPlayerTypeChanged(FPlayerTag PlayerTag);
	
};
