// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AI/Enemy_Base.h"
#include "Engine/DataTable.h"
#include "EnemyWaveTable.generated.h"

USTRUCT(BlueprintType)
struct HEXATD_API FEnemyWaveEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Count = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AEnemy_Base> EnemyClass;
};

/**
 * TODO
 */
USTRUCT(BlueprintType)
struct HEXATD_API FEnemyWaveRow : public FTableRowBase
{
	GENERATED_BODY()

	/** TODO **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FEnemyWaveEntry> EnemyCollection;
};
