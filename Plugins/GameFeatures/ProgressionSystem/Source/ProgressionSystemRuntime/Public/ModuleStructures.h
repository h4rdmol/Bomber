﻿// Copyright (c) Yevhenii Selivanov

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Bomber.h"
#include "Structures/PlayerTag.h"
#include "ModuleStructures.generated.h"


USTRUCT(BlueprintType)
struct FProgressionRowData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="C++")
	ELevelType Map;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="C++")
	FPlayerTag Character;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="C++")
	UTexture* Icon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="C++")
	FColor ActiveColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="C++")
	FColor InactiveColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="C++")
	int CurrentLevelProgression;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="C++")
	int PointsToUnlock;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="C++")
	bool IsLevelLocked;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="C++")
	int WinReward;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="C++")
	int DrawReward;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="C++")
	int LossReward;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="C++")
	float SecondsToSurvive;
};

/**
 * 
 */
UCLASS()
class PROGRESSIONSYSTEMRUNTIME_API UModuleStructures : public UObject
{
	GENERATED_BODY()
};
