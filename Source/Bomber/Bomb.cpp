// Fill out your copyright notice in the Description page of Project Settings.

#include "Bomb.h"

#include "Bomber.h"
#include "Components/StaticMeshComponent.h"
#include "GeneratedMap.h"
#include "MapComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "SingletonLibrary.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
ABomb::ABomb()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Initialize Root Component
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));

	// Initialize MapComponent
	MapComponent = CreateDefaultSubobject<UMapComponent>(TEXT("MapComponent"));

	// Initialize bomb mesh component
	BombMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BombMeshComponent"));
	BombMeshComponent->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> BombMeshFinder(TEXT("/Game/Bomber/Assets/Meshes/BombMesh"));
	if (BombMeshFinder.Succeeded())
	{
		BombMeshComponent->SetStaticMesh(BombMeshFinder.Object);
	}

	// Initialize explosion particle component
	ExplosionParticle = CreateDefaultSubobject<UParticleSystem>(TEXT("Explosion Particle"));
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleFinder(TEXT("/Game/VFX_Toolkit_V1/ParticleSystems/356Days/Par_CrescentBoom2_OLD"));
	if (ParticleFinder.Succeeded())
	{
		ExplosionParticle = ParticleFinder.Object;
	}

	// Find materials
	static TArray<ConstructorHelpers::FObjectFinder<UMaterialInterface>> MaterialsFinderArray{
		TEXT("/Game/Bomber/Assets/MI_Bombs/MI_Bomb_Yellow"),
		TEXT("/Game/Bomber/Assets/MI_Bombs/MI_Bomb_Blue"),
		TEXT("/Game/Bomber/Assets/MI_Bombs/MI_Bomb_Silver"),
		TEXT("/Game/Bomber/Assets/MI_Bombs/MI_Bomb_Pink")};
	for (int32 i = 0; i < MaterialsFinderArray.Num(); ++i)
	{
		if (MaterialsFinderArray[i].Succeeded())
		{
			BombMaterials_.Add(MaterialsFinderArray[i].Object);
		}
	}

	// Initialize Bomb Collision Component
	BombCollisionComponent = CreateDefaultSubobject<UBoxComponent>("BombCollisionComponent");
	BombCollisionComponent->SetupAttachment(RootComponent);
	BombCollisionComponent->SetBoxExtent(FVector(100.f));
}

void ABomb::InitializeBombProperties(
	int32& RefBombsN,
	const int32& FireN,
	const int32& CharacterID)
{
	if (!IS_VALID(USingletonLibrary::GetLevelMap())  // // The Level Map is not valid
		|| IS_VALID(MapComponent) == false			 // MapComponent is not valid
		|| FireN < 0)								 // Negative length of the explosion
	{
		return;
	}

	CharacterBombsN_ = &RefBombsN;
	if (CharacterBombsN_ != nullptr)
	{
		(*CharacterBombsN_)--;
	}

	// Set material
	if (IS_VALID(BombMeshComponent) == true  // Mesh of the bomb is not valid
		&& CharacterID > 0)					 // No materials for the negative ID
	{
		const int32 BombMaterialNo = FMath::Abs(CharacterID) % BombMaterials_.Num();
		BombMeshComponent->SetMaterial(0, BombMaterials_[BombMaterialNo]);
	}

	// Update explosion information
	ExplosionCells_ = USingletonLibrary::GetLevelMap()->GetSidesCells(MapComponent->Cell, FireN, EPathTypesEnum::Explosion);
}

// Called when the game starts or when spawned
void ABomb::BeginPlay()
{
	Super::BeginPlay();

	// Binding to the event, that triggered when the actor has been explicitly destroyed
	OnDestroyed.AddDynamic(this, &ABomb::OnBombDestroyed);

	// Binding to the event, that triggered when character end to overlaps the ItemCollisionComponent component
	BombCollisionComponent->OnComponentEndOverlap.AddDynamic(this, &ABomb::OnBombEndOverlap);

	// Destroy itself after N seconds
	SetLifeSpan(LifeSpan_);
}

void ABomb::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (IS_VALID(MapComponent) == false)  // this component is not valid for owner construction
	{
		return;
	}

	// Construct the actor's map component
	MapComponent->OnMapComponentConstruction();

#if WITH_EDITOR
	if (IS_PIE(GetWorld()) == true						  // for editor only
		&& USingletonLibrary::GetSingleton() != nullptr)  // Singleton is not null
	{
		InitializeBombProperties(*CharacterBombsN_, ExplosionLength, -1);
		USingletonLibrary::PrintToLog(this, "[PIE]OnConstruction", "-> \t AddDebugTextRenders");
		USingletonLibrary::AddDebugTextRenders(this, ExplosionCells_, FLinearColor::Red);
	}
#endif  //WITH_EDITOR [PIE]
}

void ABomb::OnBombDestroyed(AActor* DestroyedActor)
{
	UWorld* const World = GetWorld();
	if (World == nullptr								 // World is null
		|| !IS_VALID(USingletonLibrary::GetLevelMap()))  // The Level Map is not valid
	{
		return;
	}

	// Return to the character +1 of bombs
	if (CharacterBombsN_ != nullptr)
	{
		(*CharacterBombsN_)++;
	}
	// Spawn emitters
	for (const FCell& Cell : ExplosionCells_)
	{
		UGameplayStatics::SpawnEmitterAtLocation(World, ExplosionParticle, FTransform(GetActorRotation(), Cell.Location, GetActorScale3D()));
	}

	// Destroy all actors from array of cells
	USingletonLibrary::GetLevelMap()->DestroyActorsFromMap(ExplosionCells_);
}

void ABomb::OnBombEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == this)  // Self triggering
	{
		return;
	}

	//Sets the collision preset to block all dynamics
	TArray<AActor*> OverlappingActors;
	BombCollisionComponent->GetOverlappingActors(OverlappingActors, ACharacter::StaticClass());
	if (OverlappingActors.Num() > 0)  // There are no more characters on the bomb
	{
		BombCollisionComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	}
}
