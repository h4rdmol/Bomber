// Copyright 2021 Yevhenii Selivanov.

#include "GameFramework/MyGameUserSettings.h"
//---

//---
#include "Engine/DataTable.h"
#include "Globals/SingletonLibrary.h"

//
void USettingsDataAsset::GenerateSettingsArray(TArray<FSettingsRow>& OutRows) const
{
	if (!ensureMsgf(SettingsDataTableInternal, TEXT("ASSERT: 'SettingsDataTableInternal' is not valid")))
	{
		return;
	}

	const TMap<FName, uint8*> RowMap = SettingsDataTableInternal->GetRowMap();
	OutRows.Empty();
	OutRows.Reserve(RowMap.Num());
	for (const auto& RowIt : RowMap)
	{
		if (const auto FoundRowPtr = reinterpret_cast<FSettingsRow*>(RowIt.Value))
		{
			FSettingsRow SettingsTableRow(*FoundRowPtr);
			OutRows.Emplace(MoveTemp(SettingsTableRow));
		}
	}
}

// Get a multicast delegate that is called any time the data table changes
void USettingsDataAsset::BindOnDataTableChanged(
	const FOnDataTableChanged& EventToBind) const
{
#if WITH_EDITOR // [IsEditorNotPieWorld]
	if (!USingletonLibrary::IsEditorNotPieWorld()
	    || !SettingsDataTableInternal
	    || !EventToBind.IsBound())
	{
		return;
	}

	UDataTable::FOnDataTableChanged& OnDataTableChangedDelegate = SettingsDataTableInternal->OnDataTableChanged();
	OnDataTableChangedDelegate.AddLambda([EventToBind]()
	{
		EventToBind.Execute();
	});
#endif // WITH_EDITOR
}

//
void UMyGameUserSettings::LoadSettings(bool bForceReload)
{
	Super::LoadSettings(bForceReload);

#if WITH_EDITOR // [IsEditorNotPieWorld]
	const USettingsDataAsset* SettingsDataAsset = USingletonLibrary::GetSettingsDataAsset();
	if (USingletonLibrary::IsEditorNotPieWorld()
	    && SettingsDataAsset)
	{
		USettingsDataAsset::FOnDataTableChanged OnDataTableChanged;
		OnDataTableChanged.BindDynamic(this, &ThisClass::OnDataTableChanged);
		SettingsDataAsset->BindOnDataTableChanged(OnDataTableChanged);
	}
#endif	  // WITH_EDITOR
}

// Called whenever the data of a table has changed, this calls the OnDataTableChanged() delegate and per-row callbacks
void UMyGameUserSettings::OnDataTableChanged()
{
#if WITH_EDITOR // [IsEditorNotPieWorld]
	const USettingsDataAsset* SettingsDataAsset = USingletonLibrary::GetSettingsDataAsset();
	if (!USingletonLibrary::IsEditorNotPieWorld()
	    || !ensureMsgf(SettingsDataAsset, TEXT("ASSERT: 'SettingsDataAsset' is not valid")))
	{
		return;
	}

	TArray<FSettingsRow> SettingsTableRows;
	SettingsDataAsset->GenerateSettingsArray(SettingsTableRows);
	if (!ensureMsgf(SettingsTableRows.IsValidIndex(0), TEXT("ASSERT: 'SettingsTableRows' is empty")))
	{
		return;
	}

	//@todo fill map by [functiontag, functionptr]
	const FSettingsRow& SettingsTableRow = SettingsTableRows[0];
	const FName FunctionName = SettingsTableRow.Setter.FunctionName;
	if (!FunctionName.IsNone() && FunctionName.IsValid())
	{
		// OnOptionSelect.BindUFunction(this, FunctionName);
		// OnOptionSelect.Execute(-1);
	}
#endif	  // WITH_EDITOR
}
