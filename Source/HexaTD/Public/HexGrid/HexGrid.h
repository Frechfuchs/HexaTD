// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HexGridTypes.h"
#include "HexGrid.generated.h"

USTRUCT(BlueprintType)
struct FHexTile
{
	GENERATED_USTRUCT_BODY()

	FHexTile() {}

	FHexTile(const FHCubeCoord &InCubeCoords, FVector InWorldPosition, float InCost, bool InIsBlocking)
		: CubeCoord(InCubeCoords), WorldPosition(InWorldPosition), Cost(InCost), bIsBlocking(InIsBlocking)
	{}

	/* Coordinate in Cube space */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HexGrid")
	FHCubeCoord CubeCoord;

	/* Coordinate in World space */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HexGrid")
	FVector WorldPosition {
		FVector::ZeroVector
	};

	/* Cost of the tile, for a well execution of the GraphAStar pathfinder it need to have a value of at least 1 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HexGrid", meta =(ClampMin = 1))
	float Cost{};

	/* Is this tile a blocking tile? For example a static obstacle. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HexGrid")
	bool bIsBlocking{};

	/* Is this tile a blocking tile for building on it? For example goal or spawn points */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HexGrid")
	bool bIsBlockingForPlayerOnly{};


	friend bool operator==(const FHexTile &A, const FHexTile &B)
	{
		return (A.CubeCoord == B.CubeCoord) && (A.Cost == B.Cost) && (A.bIsBlocking == B.bIsBlocking);
	}

	friend bool operator!=(const FHexTile &A, const FHexTile &B)
	{
		return !(A == B);
	}

};

UCLASS()
class HEXATD_API AHexGrid : public AActor
{
	GENERATED_BODY()
	
public:	
	/* Sets default values for this actor's properties */
	AHexGrid();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/**
	 * Layout of the tile
	 * @see  https://www.redblobgames.com/grids/hexagons/implementation.html#layout
	 */ 
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "HexGrid")
	FHTileLayout TileLayout = FHTileLayout(EHTileOrientationFlag::FLAT, 100.f, FVector(0.f, 0.f, 0.f));

	/** Radius of the grid in tiles */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "HexGrid|MapShape|Hexagon",
		meta = (ClampMin = "1", ClampMax = "100", UIMin = "1", UIMax = "100"))
	int32 Radius = 4;

	/** Amount of tile to the right, when using rectangle MapShape */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "HexGrid|MapShape|Rectangle",
		meta = (ClampMin = "1", ClampMax = "100", UIMin = "1", UIMax = "100"))
	int32 RectangleRight = 10;

	/** Amount of tile to the bottom, when using rectangle MapShape */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "HexGrid|MapShape|Rectangle",
		meta = (ClampMin = "1", ClampMax = "100", UIMin = "1", UIMax = "100"))
	int32 RectangleBottom = 10;

	/** InstancedStaticMesh for the regular tile */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "HexGrid|PRIVATE|Mesh")
	UInstancedStaticMeshComponent* RegularTile;

	/** Array of HexTiles */
	UPROPERTY(BlueprintReadWrite, Category = "HexGrid")
	TArray<FHexTile> GridTiles {};

	/** Array of Cube coordinates that compose the grid. */
	UPROPERTY(BlueprintReadWrite, Category = "HexGrid")
	TArray<FHCubeCoord> GridCoordinates {};

	/**
	 * Return the neighbor Cube coordinate in the provided direction. 
	 * @see https://www.redblobgames.com/grids/hexagons/#neighbors
	 */
	UFUNCTION(BlueprintCallable, Category = "HexGrid")
	FHCubeCoord GetNeighbor(const FHCubeCoord &H, const FHCubeCoord &Dir);

	/**
	 * Return one of the six cube directions.
	 * @see https://www.redblobgames.com/grids/hexagons/#neighbors
	 */
	UFUNCTION(BlueprintCallable, Category = "HexGrid")
	FHCubeCoord GetDirection(int32 Dir);

	/**
	 * Convert coordinates from World space to Cube space.
	 * @see https://www.redblobgames.com/grids/hexagons/#pixel-to-hex
	 */
	UFUNCTION(BlueprintCallable, Category = "HexGrid")
	FHCubeCoord WorldToHex(const FVector &Location);

	/** Translates a given CubeCoord to WorldPosition */
	UFUNCTION(BlueprintCallable, Category = "HexGrid")
	FVector HexToWorld(const FHCubeCoord &CC);

	/** Snap a World coordinate to the Grid space. */
	UFUNCTION(BlueprintCallable, Category = "HexGrid")
	FVector SnapToGrid(const FVector &Location, bool &IsValid);

	/** Make Tile occupied
	 * TODO: Needs refactor, it should accept a CubeCoord instead of FVector
	 */
	UFUNCTION(BlueprintCallable, Category = "HexGrid")
	void OccupieGridSpace(const FVector &Location, bool &Success);

	/** Make Tile occupied for player only
	 * TODO: Needs refactor, it should accept a CubeCoord instead of FVector
	 */
	UFUNCTION(BlueprintCallable, Category = "HexGrid")
	void OccupieGridSpacePlayerOnly(const FVector &Location, bool &Success);

	/** Free Tile from being occupied
	 * TODO: Needs refactor, it should accept a CubeCoord instead of FVector
	 */
	UFUNCTION(BlueprintCallable, Category = "HexGrid")
	void FreeOccupiedGridSpace(const FVector &Location, bool &Success);

protected:
	/* Called when the game starts or when spawned */
	virtual void BeginPlay() override;

	/* Called when actor is constructed or properties change */
	virtual void OnConstruction(const FTransform& Transform) override;

private:
	/** Creates the grid depend on TileLayout and calls AddTile for it */
	void CreateGrid();

	/** Creates a hexagon-shaped grid and adds it to GridCoordinates */
	void CreateGridHexagon();

	/** Creates a rectangle-shaped grid with flat tiles and adds it to GridCoordinates */
	void CreateGridRectangleFlat();

	/** Creates a rectangle-shaped grid with pointy tiles and adds it to GridCoordinates */
	void CreateGridRectanglePointy();

	/** Add an instance of RegularTile for the given CubeCoord */
	void AddTile(const FHCubeCoord &CubeCoord);

	/** 
	 * Round from floating-point cube coordinate to integer cube coordinate. 
	 * @see https://www.redblobgames.com/grids/hexagons/#rounding
	 */
	FHCubeCoord RoundHex(const FHFractional &F);

	/** Calculate tile transform for a given location */
	FTransform CalculateTransform(FVector TileLocation);

	/** Find a Tile by given coord */
	FHexTile* GetHexTileByCubeCoord(const FHCubeCoord &CubeCoord);

	// TODO
	FHDirections HDirections{};
};
