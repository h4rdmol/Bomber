// Copyright (c) Yevhenii Selivanov

#pragma once

#include "GameFramework/PlayerState.h"
//---
#include "Bomber.h"
//---
#include "MyPlayerState.generated.h"

/**
 * The player state of a bomber player.
 */
UCLASS(Config = "GameUserSettings", DefaultConfig)
class BOMBER_API AMyPlayerState final : public APlayerState
{
	GENERATED_BODY()

public:
	/** Default constructor. */
	AMyPlayerState();

	/** Returns true if this Player State is controlled by a locally controlled player. */
	UFUNCTION(BlueprintPure, Category = "C++")
	bool IsPlayerStateLocallyControlled() const;

	/** Returns always valid owner (human or bot), or crash if nullptr. */
	class APlayerCharacter& GetPlayerCharacterChecked() const;

	/*********************************************************************************************
	 * End Game State
	 * Is personal for each player: Win, Lose or Draw.
	 * Can be tracked by listening UGlobalEventsSubsystem::Get().OnEndGameStateChanged
	 ********************************************************************************************* */
public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEndGameStateChanged, EEndGameState, EndGameState);

	/** Called when player's match result was changed (Win, lose, draw or none applied). */
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Transient, Category = "C++")
	FOnEndGameStateChanged OnEndGameStateChanged;

	/** Returns result of the game for controlled player after ending the game. */
	UFUNCTION(BlueprintPure, Category = "C++")
	FORCEINLINE EEndGameState GetEndGameState() const { return EndGameStateInternal; }

	/** Updates result of the game for controlled player. */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "C++")
	void UpdateEndGameState();

protected:
	/** Contains result of the game for controlled player after ending the game. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Transient, Replicated, Category = "C++", meta = (BlueprintProtected, DisplayName = "End Game State"))
	EEndGameState EndGameStateInternal = EEndGameState::None;

	/** Set new End-Game state, is made as multicast to notify own client asap. */
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable, Category = "C++", meta = (BlueprintProtected))
	void MulticastSetEndGameState(EEndGameState NewEndGameState);

	/***************************************************************************************************************************************
	 * Nickname
	 * - Base::SetPlayerName(Name) for any (Human/AI)
	 * - This::SetDefaultAIName() for AI
	 * - This::SetSavedPlayerName(Name) for human
	 * _____________________________________________________________________________________________________________________________________
	 * | Variable                | Getter Function       | Class | Applies | Cfg | Rep | Description                                       |
	 * |-------------------------|-----------------------|-------|---------|-----|-----|---------------------------------------------------|
	 * | PlayerNamePrivate       | GetPlayerName()       | Base  | Human/AI| -   | +   | Best method to obtain current nickname            |
	 * | OldNamePrivate          | GetPendingPlayerName()| This  | Human   | -   | -   | Pending changed nickname in settings char by char |
	 * | SavedPlayerNameInternal | GetSavedPlayerName()  | This  | Human   | +   | +   | Saved nickname in game settings                   |
	 **************************************************************************************************************************************/
public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerNameChanged, FName, NewName);

	/** Called when player name is changed. */
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Transient, Category = "C++")
	FOnPlayerNameChanged OnPlayerNameChanged;

	/** Exposes Base::SetOldPlayerName() to blueprints to set locally the player name on each nickname change.*/
	UFUNCTION(BlueprintCallable, Category = "C++")
	void SetPendingPlayerName(FName NewName) { SetOldPlayerName(NewName.ToString()); }

	/** Exposes Base::GetOldPlayerName() to blueprints to get locally the player name on each nickname change.*/
	UFUNCTION(BlueprintPure, Category = "C++")
	FORCEINLINE FName GetPendingPlayerName() const { return *GetOldPlayerName(); }

	/** Sets saved human name to config property.
	 * SaveConfig() needs to be called separately to save it to the file. */
	UFUNCTION(BlueprintCallable, Category = "C++")
	void SetSavedPlayerName(FName NewName);

	/** Returns saved human name from config file. */
	UFUNCTION(BlueprintPure, Category = "C++")
	FORCEINLINE FName GetSavedPlayerName() const { return SavedPlayerNameInternal; }

	/** Applies default bots name based on current character ID like "AI 0", "AI 1" etc. */
	UFUNCTION(BlueprintCallable, Category = "C++")
	void SetDefaultAIName();

	/** Is overridden to additionally set player name on server and broadcast it. */
	virtual void SetPlayerName(const FString& S) override;

	/** Applies and broadcasts player name. */
	UFUNCTION(BlueprintCallable, Category = "C++")
	void ApplyPlayerName();

protected:
	/** Is created for saving purposes since base APlayerState::PlayerNamePrivate property is not 'Config'.
	 * Can contain different languages, uppercase, lowercase etc, is config property. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Config, Category = "C++", meta = (BlueprintProtected, DisplayName = "Saved Player Name"))
	FName SavedPlayerNameInternal;

	/** Called on server when settings are saved to apply local player name. */
	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "C++", meta = (BlueprintProtected))
	void ServerSetPlayerName(FName NewName);

	/** Called on client when player name is changed. */
	virtual void OnRep_PlayerName() override;

	/*********************************************************************************************
	 * Is Character Dead
	 ********************************************************************************************* */
public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterDeadChanged, bool, bIsCharacterDead);

	/** Called when character dead status is changed: character was killed or revived. */
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Transient, Category = "C++")
	FOnCharacterDeadChanged OnCharacterDeadChanged;

	/** Returns true if current player is alive.*/
	UFUNCTION(BlueprintPure, Category = "C++")
	FORCEINLINE bool IsCharacterDead() const { return bIsCharacterDeadInternal; }

	/** Sets character dead status, true if was killed, false if was revived. */
	UFUNCTION(BlueprintCallable, Category = "C++")
	void SetCharacterDead(bool bIsDead);

protected:
	/** Is true when player is dead. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Transient, ReplicatedUsing = "OnRep_IsCharacterDead", Category = "C++", meta = (BlueprintProtected, DisplayName = "Is Character Dead"))
	bool bIsCharacterDeadInternal = false;

	/** Called on client when character Dead status is changed. */
	UFUNCTION()
	void OnRep_IsCharacterDead();

	/** Applies and broadcasts Is Character Dead status. */
	UFUNCTION(BlueprintCallable, Category = "C++", meta = (BlueprintProtected))
	void ApplyIsCharacterDead();

	/*********************************************************************************************
	 * Is Human / Bot
	 * APlayerState::bIsABot is used to determine if the player is a bot.
	 ********************************************************************************************* */
public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnIsABotChanged, bool, bIsABot);

	/** Called when player is changed from human to bot or vice versa. */
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Transient, Category = "C++")
	FOnIsABotChanged OnIsABotChanged;

	/** Applies bot status, overloads engine's APlayerState::SetIsABot(bool) that is not virtual and not exposed to blueprints. */
	UFUNCTION(BlueprintCallable, Category = "C++")
	void SetIsABot();

	/** Applies human status. */
	UFUNCTION(BlueprintCallable, Category = "C++")
	void SetIsHuman();

protected:
	/** Called on client when APlayerState::bIsABot is changed. */
	UFUNCTION()
	void OnRep_IsABot();

	/** Applies and broadcasts IsABot status. */
	UFUNCTION(BlueprintCallable, Category = "C++", meta = (BlueprintProtected))
	void ApplyIsABot();

	/*********************************************************************************************
	 * Events
	 ********************************************************************************************* */
public:
	/** Is called when player state is initialized with assigned character. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "C++")
	void OnPlayerStateInit();

	/** Listens game states to notify server about ending game for controlled player. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "C++", meta = (BlueprintProtected))
	void OnGameStateChanged(ECurrentGameState CurrentGameState);

	/** Listens game settings to apply them once saved. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "C++", meta = (BlueprintProtected))
	void OnSaveSettings();

	/*********************************************************************************************
	 * Overrides
	 ********************************************************************************************* */
protected:
	/** Returns properties that are replicated for the lifetime of the actor channel. */
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	/** Called when the game starts. */
	virtual void BeginPlay() override;

	/** Register a player with the online subsystem. */
	virtual void RegisterPlayerWithSession(bool bWasFromInvite) override;

	/** Unregister a player with the online subsystem. */
	virtual void UnregisterPlayerWithSession() override;

	/** Is overridden to handle own OnRep functions for engine properties.
	 * Called right after calling all OnRep notifies (called even when there are no notifies). */
	virtual void PostRepNotifies() override;
};