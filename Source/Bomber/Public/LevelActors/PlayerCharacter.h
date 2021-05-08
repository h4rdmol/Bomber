﻿// Copyright 2021 Yevhenii Selivanov.

#pragma once

#include "GameFramework/Character.h"
//---
#include "Components/MySkeletalMeshComponent.h"
#include "Globals/LevelActorDataAsset.h"
//---
#include "PlayerCharacter.generated.h"

/**
 * Determines each mesh to attach.
 */
USTRUCT(BlueprintType)
struct FAttachedMesh
{
	GENERATED_BODY()

	/** The attached static mesh or skeletal mesh.  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ShowOnlyInnerProperties, ExposeOnSpawn))
	class UStreamableRenderAsset* AttachedMesh = nullptr; //[D]

	/** In the which socket should attach this prop. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ShowOnlyInnerProperties))
	FName Socket = NAME_None; //[D]

	/** Prop animation is loop played all the time, starts playing on attaching to the owner. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Row", meta = (ShowOnlyInnerProperties))
	class UAnimSequence* MeshAnimation; //[D]
};

/**
 * The player archetype of level actor rows. Determines the individual of the character model
 */
UCLASS(Blueprintable, BlueprintType)
class UPlayerRow final : public ULevelActorRow
{
	GENERATED_BODY()

public:
	/** All meshes that will be attached to the player. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Row", meta = (ShowOnlyInnerProperties))
	TArray<FAttachedMesh> PlayerProps; //[D]

	/** The own movement animation for the each character. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Row", meta = (ShowOnlyInnerProperties))
	class UBlendSpace1D* IdleWalkRunBlendSpace; //[D]

	/** Dance animation that is used mostly in the menu instead of idle. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Row", meta = (ShowOnlyInnerProperties))
	class UAnimSequence* DanceAnimation; //[D]

	/** Returns the num of skin textures in the array of diffuse maps specified a player material instance. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "C++")
	FORCEINLINE int32 GetMaterialInstancesDynamicNum() const { return MaterialInstancesDynamicInternal.Num(); }

	/** Returns the dynamic material instance of a player with specified skin.
	 * @param SkinIndex The skin position to get.
	 * @see UPlayerRow::MaterialInstancesDynamicInternal */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "C++")
	class UMaterialInstanceDynamic* GetMaterialInstanceDynamic(int32 SkinIndex) const;

protected:
	/** The material instance of a player.
	 * @warning Is not BlueprintReadOnly and has not getter to prevent being used directly, we have dynamic materials instead.
	 * @see UPlayerRow::MaterialInstancesDynamicInternal. */
	UPROPERTY(EditDefaultsOnly, Category = "Row", meta = (BlueprintProtected, DisplayName = "Material Instance", ShowOnlyInnerProperties))
	class UMaterialInstance* MaterialInstanceInternal; //[D]

	/**
	 * Contains all created dynamic materials for each skin in the Material Instance.
	 * Saves memory avoiding creation of dynamic materials for each mesh component, just use the same dynamic material for different meshes with the same skin.
	 * Is filled on object creating and changing.
	 * @warning Is not EditDefaultsOnly because have to be created dynamically, in the same time is incompatible with VisibleInstanceOnly.
	 * @see UPlayerRow::MaterialInstanceInternal. */
	UPROPERTY(BlueprintReadOnly, Category = "C++", meta = (BlueprintProtected, DisplayName = "Material Instances Dynamic"))
	TArray<class UMaterialInstanceDynamic*> MaterialInstancesDynamicInternal; //[G]

#if WITH_EDITOR
	/** Handle adding and changing material instance to prepare dynamic materials. */
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif	//WITH_EDITOR

	/** Called after loading an object. Is overridden to prepare dynamic materials. */
	virtual void PostLoad() override;

	/**
	 * Create dynamic material instance for each ski if is not done before.
	 * UPlayerRow::MaterialInstancesDynamicInternal
	 */
	UFUNCTION(BlueprintCallable, Category = "C++", meta = (BlueprintProtected))
	void TryCreateDynamicMaterials();
};

/**
 * The data asset of the Bomber characters
 */
UCLASS(Blueprintable, BlueprintType)
class UPlayerDataAsset final : public ULevelActorDataAsset
{
	GENERATED_BODY()

public:
	/** Default constructor. */
	UPlayerDataAsset();

	/** Returns the player data asset. */
	static const UPlayerDataAsset& Get();

	/** The num of nameplate materials.  */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "C++")
	FORCEINLINE int32 GetNameplateMaterialsNum() const { return NameplateMaterialsInternal.Num(); }

	/** Returns a nameplate material by index, is used by nameplate meshes.
	 * @see UPlayerDataAsset::NameplateMaterials */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "C++")
	class UMaterialInterface* GetNameplateMaterial(int32 Index) const;

	/** Returns the Anim Blueprint class to use.
	 * @see UPlayerDataAsset::AnimInstanceClassInternal. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "C++")
	FORCEINLINE TSubclassOf<UAnimInstance> GetAnimInstanceClass() const { return AnimInstanceClassInternal; }

	/** Returns the name of a material parameter with a diffuse array.
	 * @see UPlayerDataAsset::SkinSlotNameInternal. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "C++")
	FORCEINLINE FName GetSkinArrayParameter() const { return SkinArrayParameterInternal; }

	/** Returns the name of a material parameter with a diffuse index.
	* @see UPlayerDataAsset::SkinSlotNameInternal. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "C++")
	FORCEINLINE FName GetSkinIndexParameter() const { return SkinIndexParameterInternal; }

protected:
	/** All materials that are used by nameplate meshes. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "C++", meta = (BlueprintProtected, DisplayName = "Nameplate Materials", ShowOnlyInnerProperties))
	TArray<class UMaterialInterface*> NameplateMaterialsInternal; //[D]

	/** The AnimBlueprint class to use, can set it only in the gameplay. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "C++", meta = (BlueprintProtected, DisplayName = "Anim Instance Class", ShowOnlyInnerProperties))
	TSubclassOf<UAnimInstance> AnimInstanceClassInternal; //[D]

	/** The name of a material parameter with a diffuse array. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "C++", meta = (BlueprintProtected, DisplayName = "Skin Array Parameter", ShowOnlyInnerProperties))
	FName SkinArrayParameterInternal = "DiffuseArray"; //[D]

	/** The name of a material parameter with a diffuse index. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "C++", meta = (BlueprintProtected, DisplayName = "Skin Index Parameter", ShowOnlyInnerProperties))
	FName SkinIndexParameterInternal = "DiffuseIndex"; //[D]
};

/**
 * Numbers of power-ups that affect the abilities of a player during gameplay.
 * @todo rewrite as attributes of ability system
 */
USTRUCT(BlueprintType)
struct FPowerUp
{
	GENERATED_BODY()

	/** The number of items, that increases the movement speed of the character */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "C++")
	int32 SkateN = 1;

	/** The number of bombs that can be set at one time */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "C++")
	int32 BombN = 1;

	/** The number of items, that increases the bomb blast radius */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "C++")
	int32 FireN = 1;
};

/**
 * Players and AI, whose goal is to remain the last survivor for the win.
 */
UCLASS()
class APlayerCharacter final : public ACharacter
{
	GENERATED_BODY()

public:
	/** ---------------------------------------------------
	 *		Public functions
	 * --------------------------------------------------- */

	/** Sets default values for this character's properties */
	APlayerCharacter(const FObjectInitializer& ObjectInitializer);

	/** Returns current powerup levels */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "C++")
	FORCEINLINE FPowerUp GetPowerups() const { return PowerupsInternal; }

	/** Returns the personal ID. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "C++")
	FORCEINLINE int32 GetCharacterID() const { return CharacterIDInternal; }

	/** Finds and rotates the self at the current character's location to point at the specified location.
	 * @param Location the character is looking at.
	 * @param bShouldInterpolate if true, smoothly rotate the character toward the direction. */
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "C++", meta = (AutoCreateRefTerm = "Location"))
	void RotateToLocation(const FVector& Location, bool bShouldInterpolate) const;

	/** Spawns bomb on character position */
	UFUNCTION(BlueprintCallable, Category = "C++")
	void SpawnBomb();

	/** Set and apply new skeletal mesh by specified data.
	 * @param CustomPlayerMeshData Contains data about the skin to set. */
	UFUNCTION(BlueprintCallable, Category = "C++")
	void InitMySkeletalMesh(const FCustomPlayerMeshData& CustomPlayerMeshData);

protected:
	/** ---------------------------------------------------
	 *		Protected properties
	 * --------------------------------------------------- */

	friend class UMyCheatManager;

	/** The MapComponent manages this actor on the Level Map */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "C++", meta = (BlueprintProtected, DisplayName = "Map Component"))
	class UMapComponent* MapComponentInternal; //[C.AW]

	/** The static mesh nameplate */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "C++", meta = (BlueprintProtected, DisplayName = "Nameplate Mesh Component"))
	class UStaticMeshComponent* NameplateMeshInternal; //[C.DO]

	/** Count of items that affect on a player during gameplay. Can be overriden by the Cheat Manager. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "C++", meta = (BlueprintProtected, DisplayName = "Powerups", ShowOnlyInnerProperties))
	FPowerUp PowerupsInternal; //[AW]

	/** The ID identification of each character */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "C++", meta = (BlueprintProtected, DisplayName = "Character ID"))
	int32 CharacterIDInternal = INDEX_NONE; //[G]

	/** The character's AI controller */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "C++", meta = (BlueprintProtected, DisplayName = "My AI Controller"))
	class AMyAIController* MyAIControllerInternal; //[G]

	/** Store to pause and unpause updating the current location of the player and bots on the level map.  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "C++", meta = (BlueprintProtected, DisplayName = "AI Update Handle"))
	FTimerHandle UpdatePositionHandleInternal;

	/** ---------------------------------------------------
	 *		Protected functions
	 * --------------------------------------------------- */

	/** Called when the game starts or when spawned */
	virtual void BeginPlay() override;

	/** Called when an instance of this class is placed (in editor) or spawned */
	virtual void OnConstruction(const FTransform& Transform) override;

	/** ---------------------------------------------------
	 *		Input
	 * --------------------------------------------------- */

	/** Called to bind functionality to input */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Adds the movement input along the given world direction vector.
	 *
	 * @param WorldDirection Direction in world space to apply input
	 * @param ScaleValue Scale to apply to input. This can be used for analog input, ie a value of 0.5 applies half the normal value, while -1.0 would reverse the direction.
	 * @param bForce If true always add the input, ignoring the result of IsMoveInputIgnored().
	 */
	virtual void AddMovementInput(FVector WorldDirection, float ScaleValue = 1.f, bool bForce = false) override;

	/** Move the player character by the forward vector. */
	UFUNCTION(BlueprintCallable, Category = "C++", meta = (BlueprintProtected))
	void MoveUpDown(float ScaleValue);

	/** Move the player character by the right vector. */
	UFUNCTION(BlueprintCallable, Category = "C++", meta = (BlueprintProtected))
	void MoveRightLeft(float ScaleValue);

	/** --------------------------------------------------- */

	/**
	 * Triggers when this player character starts something overlap.
	 * With item overlapping Increases +1 to numbers of character's powerups (Skate/Bomb/Fire).
	 */
	UFUNCTION(BlueprintCallable, Category = "C++", meta = (BlueprintProtected))
	void OnPlayerBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

	/** Event triggered when the bomb has been explicitly destroyed. */
	UFUNCTION(BlueprintCallable, Category = "C++", meta = (BlueprintProtected))
	void OnBombDestroyed(AActor* DestroyedBomb);

	/** Called when the current game state was changed. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "C++", meta = (BlueprintProtected))
	void OnGameStateChanged(ECurrentGameState CurrentGameState);

	/** Update player name on a 3D widget component. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, BlueprintPure = false, Category = "C++")
	void UpdateNickname() const;
};
