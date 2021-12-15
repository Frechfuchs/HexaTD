// Fill out your copyright notice in the Description page of Project Settings.


#include "HexGrid/HexGrid.h"

/* Sets default values for this actor's properties */
AHexGrid::AHexGrid()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	// Add RegularTileComponent
	RegularTile = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("ISM_RegularTile"));
	RegularTile->SetupAttachment(RootComponent);
}

/* Called when the game starts or when spawned */
void AHexGrid::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AHexGrid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

/* Called when actor is constructed or properties change */
void AHexGrid::OnConstruction(const FTransform& Transform)
{
	// Clear internal grid arrays
	GridTiles.Empty();
	GridCoordinates.Empty();

	// Clear all instances of RegularTile
	RegularTile->ClearInstances();

	// Set origin to actor's location
	TileLayout.Origin = GetActorLocation();

	CreateGrid();
}

/** Creates a hexagon-shaped grid and adds it to GridCoordinates */
void AHexGrid::CreateGrid()
{
	// 
	switch (TileLayout.MapShape)
	{
		case EHMapShape::HEXAGON:
			CreateGridHexagon();
			break;

		case EHMapShape::RECTANGLE:
			switch (TileLayout.TileOrientation)
			{
				case EHTileOrientationFlag::FLAT:
					CreateGridRectangleFlat();
					break;

				case EHTileOrientationFlag::POINTY:
					CreateGridRectanglePointy();
					break;
				case EHTileOrientationFlag::NONE:
					CreateGridRectanglePointy();
					break;
			}
			break;
	}

	// Add tiles for each grid-element
	for (FHCubeCoord CubeCoord : GridCoordinates)
	{
		AddTile(CubeCoord);
	}
}

/** Creates a hexagon-shaped grid and adds it to GridCoordinates */
void AHexGrid::CreateGridHexagon()
{
	/* https://www.unrealengine.com/en-US/blog/optimizing-tarray-usage-for-performance
	preallocate array memory
	R1 = 1 + 6*1
	R2 = 1 + 6*1 + 6*2
	R3 = 1 + 6*1 + 6*2 + 6*3
	R4 = 1 + 6*1 + 6*2 + 6*3 + 6*4
	R5 = ....... */
	int32 Size{ 1 };
	
	for (int32 i{ 1 }; i <= Radius; ++i)
	{
		Size += 6 * i;
	}
	GridCoordinates.Reserve(Size);
	
	for (int32 Q{ -Radius }; Q <= Radius; ++Q)
	{
		// Calculate R1
		int32 R1{ FMath::Max(-Radius, -Q - Radius) };

		// Calculate R2
		int32 R2{ FMath::Min(Radius, -Q + Radius) };

		for (int32 R{ R1 }; R <= R2; ++R)
		{
			FHCubeCoord CubeCoord{ FIntVector(Q, R, -Q - R) };
			GridCoordinates.Add(CubeCoord);
		}
	}
}

/** Creates a rectangle-shaped grid with flat tiles and adds it to GridCoordinates */
void AHexGrid::CreateGridRectangleFlat()
{
	/* https://www.unrealengine.com/en-US/blog/optimizing-tarray-usage-for-performance
	preallocate array memory
	RR1, RB1 = (1 + 1) * (1 + 1)
	RR1, RB2 = (1 + 1) * (2 + 1)
	RR2, RB1 = (2 + 1) * (1 + 1)
	RR2, RB2 = ....... */
	int32 Size{ (RectangleRight + 1) * (RectangleBottom + 1) };
	GridCoordinates.Reserve(Size);

	for (int Q = 0; Q <= RectangleRight; Q++) {
		int QOffset = floor(Q/2.0);

		for (int R = 0 - QOffset; R <= RectangleBottom - QOffset; R++) {
			FHCubeCoord CubeCoord{ FIntVector(Q, R, -Q - R) };
			GridCoordinates.Add(CubeCoord);
		}
	}
}

/** Creates a rectangle-shaped grid with pointy tiles and adds it to GridCoordinates */
void AHexGrid::CreateGridRectanglePointy()
{
	/* https://www.unrealengine.com/en-US/blog/optimizing-tarray-usage-for-performance
	preallocate array memory
	RR1, RB1 = (1 + 1) * (1 + 1)
	RR1, RB2 = (1 + 1) * (2 + 1)
	RR2, RB1 = (2 + 1) * (1 + 1)
	RR2, RB2 = ....... */
	int32 Size{ (RectangleRight + 1) * (RectangleBottom + 1) };
	GridCoordinates.Reserve(Size);

	for (int R = 0; R <= RectangleBottom; R++) {
		int ROffset = floor(R/2.0);

		for (int Q = 0 - ROffset; Q <= RectangleRight - ROffset; Q++) {
			FHCubeCoord CubeCoord{ FIntVector(Q, R, -Q - R) };
			GridCoordinates.Add(CubeCoord);
		}
	}
}

/** Add an instance of RegularTile for the given CubeCoord */
void AHexGrid::AddTile(const FHCubeCoord &CubeCoord)
{
	// Create HexTile
	FVector TileLocation = HexToWorld(CubeCoord);
	float TileCost = 1.f;
	bool bIsBlocking = false;
	FHexTile HexTile = FHexTile(CubeCoord, TileLocation, TileCost, bIsBlocking);

	// Add to GridTiles
	GridTiles.Add(HexTile);

	// Draw tile
	FTransform TileTransform = CalculateTransform(TileLocation);
	RegularTile->AddInstanceWorldSpace(TileTransform);
}

/** Translates a given CubeCoord to WorldPosition */
FVector AHexGrid::HexToWorld(const FHCubeCoord &CC)
{
	// Set the layout orientation
	FHTileOrientation TileOrientation{};
	if (TileLayout.TileOrientation == EHTileOrientationFlag::FLAT)
	{
		TileOrientation = HFlatTopLayout;
	}
	else
	{
		TileOrientation = HPointyLayout;
	}

	float x = ((TileOrientation.f0 * CC.QRS.X) + (TileOrientation.f1 * CC.QRS.Y)) * TileLayout.TileSize;
	float y = ((TileOrientation.f2 * CC.QRS.X) + (TileOrientation.f3 * CC.QRS.Y)) * TileLayout.TileSize;

	return FVector(x + TileLayout.Origin.X, y + TileLayout.Origin.Y, TileLayout.Origin.Z);
}

/**
 * Convert coordinates from World space to Cube space.
 * @see https://www.redblobgames.com/grids/hexagons/#pixel-to-hex
 */ 
FHCubeCoord AHexGrid::WorldToHex(const FVector &Location)
{
	// Set the layout orientation
	FHTileOrientation TileOrientation{};
	switch (TileLayout.TileOrientation)
	{
		case EHTileOrientationFlag::FLAT:
			TileOrientation = HFlatTopLayout;
			break;
		case EHTileOrientationFlag::POINTY:
			TileOrientation = HPointyLayout;
			break;
		case EHTileOrientationFlag::NONE:
			TileOrientation = HPointyLayout;
			break;
	}

	FVector InternalLocation{ FVector((Location.X - TileLayout.Origin.X) / TileLayout.TileSize,
									  (Location.Y - TileLayout.Origin.Y) / TileLayout.TileSize,
									  (Location.Z - TileLayout.Origin.Z))	// Z is useless here.
	};

	float q = ((TileOrientation.b0 * InternalLocation.X) + (TileOrientation.b1 * InternalLocation.Y));
	float r = ((TileOrientation.b2 * InternalLocation.X) + (TileOrientation.b3 * InternalLocation.Y));
	
	FVector v{ (TileLayout.TileOrientation == EHTileOrientationFlag::FLAT) ? FVector(q, -q - r, r) : FVector(q, r, -q - r) };

	return RoundHex(FHFractional(v));
}

/** 
 * Round from floating-point cube coordinate to integer cube coordinate. 
 * @see https://www.redblobgames.com/grids/hexagons/#rounding
 */
FHCubeCoord AHexGrid::RoundHex(const FHFractional &F)
{
	int32 q{ int32(FMath::RoundToDouble(F.QRS.X)) };
	int32 r{ int32(FMath::RoundToDouble(F.QRS.Y)) };
	int32 s{ int32(FMath::RoundToDouble(F.QRS.Z)) };

	float q_diff{ FMath::Abs(q - F.QRS.X) };
	float r_diff{ FMath::Abs(r - F.QRS.Y) };
	float s_diff{ FMath::Abs(s - F.QRS.Z) };

	if ((q_diff > r_diff) && (q_diff > s_diff))
	{
		q = -r - s;
	}
	else if (r_diff > s_diff)
	{
		r = -q - s;
	}
	else
	{
		s = -q - r;
	}

	return FHCubeCoord{ FIntVector(q, r, s) };
}

/** Calculate tile transform for a given location */
FTransform AHexGrid::CalculateTransform(FVector TileLocation)
{
	FRotator Rotation;
	switch(TileLayout.TileOrientation)
	{
		case EHTileOrientationFlag::FLAT:
			Rotation = FRotator(0.f);
			break;
		case EHTileOrientationFlag::POINTY:
			Rotation = FRotator(0.f, 30.f, 0.f);
			break;
		case EHTileOrientationFlag::NONE:
			Rotation = FRotator(0.f);
			break;
	}

	FVector Scale = FVector(1.f, 1.f, 1.f);

	return FTransform(Rotation, TileLocation, Scale);
}

/** Find a Tile by given coord 
 * TODO: Use TMap instead of TArray
*/
FHexTile* AHexGrid::GetHexTileByCubeCoord(const FHCubeCoord &CubeCoord)
{
	return GridTiles.FindByPredicate([CubeCoord](FHexTile Tile){
		return Tile.CubeCoord == CubeCoord; 
	});
}

/** PUBLIC 
 * Snap a World coordinate to the Grid space. */
FVector AHexGrid::SnapToGrid(const FVector &Location, bool &IsValid)
{
	float TempZ{ Location.Z };
	FHCubeCoord CC = WorldToHex(Location);

	FHexTile* Tile = GetHexTileByCubeCoord(CC);
	if (Tile)
	{
		IsValid = !(Tile->bIsBlocking || Tile->bIsBlockingForPlayerOnly);
	}
	else
	{
		IsValid = false;
	}

	FVector Result{ HexToWorld(CC) };
	Result.Z = TempZ;
	return Result;
}

/** Make Tile Occupied
 * TODO: Needs refactor, it should accept a CubeCoord instead of FVector
 */
void AHexGrid::OccupieGridSpace(const FVector &Location, bool &Success)
{
	FHCubeCoord CC = WorldToHex(Location);
	FHexTile* Tile = GetHexTileByCubeCoord(CC);
	if (Tile && !Tile->bIsBlocking)
	{
		Tile->Cost = 10.f;
		Tile->bIsBlocking = true;
		Success = true;
	}
	else
	{
		Success = false;
	}
}

/** Make Tile Occupied
 * TODO: Needs refactor, it should accept a CubeCoord instead of FVector
 */
void AHexGrid::OccupieGridSpacePlayerOnly(const FVector &Location, bool &Success)
{
	FHCubeCoord CC = WorldToHex(Location);
	FHexTile* Tile = GetHexTileByCubeCoord(CC);
	if (Tile && !Tile->bIsBlocking && !Tile->bIsBlockingForPlayerOnly)
	{
		Tile->bIsBlockingForPlayerOnly = true;
		Success = true;
	}
	else
	{
		Success = false;
	}
}


/** Free Tile from being occupied
 * TODO: Needs refactor, it should accept a CubeCoord instead of FVector
 * TODO: Should this free from bIsBlockingForPlayerOnly too?
 */
void AHexGrid::FreeOccupiedGridSpace(const FVector &Location, bool &Success)
{
	FHCubeCoord CC = WorldToHex(Location);
	FHexTile* Tile = GetHexTileByCubeCoord(CC);
	if (Tile && Tile->bIsBlocking)
	{
		Tile->Cost = 1.f;
		Tile->bIsBlocking = false;
		Success = true;
	}
	else
	{
		Success = false;
	}
}

FHCubeCoord AHexGrid::GetNeighbor(const FHCubeCoord &H, const FHCubeCoord &Dir)
{
	return H + Dir;
}

FHCubeCoord AHexGrid::GetDirection(int32 Dir)
{
	check(Dir < HDirections.Directions.Num());
	return HDirections.Directions[Dir];
}
