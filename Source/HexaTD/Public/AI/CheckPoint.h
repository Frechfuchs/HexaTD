// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CheckPoint.generated.h"

class UArrowComponent;

UCLASS(Blueprintable)
class UCheckPointsObject : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	TArray<FVector> CheckPoints;
};

UCLASS()
class HEXATD_API ACheckPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACheckPoint();

	// Operators
	friend bool operator<(const ACheckPoint& lhs, const ACheckPoint& rhs)
	{
		return lhs.OrderNum < rhs.OrderNum;
	}
	friend bool operator>(const ACheckPoint& lhs, const ACheckPoint& rhs)
	{
		return lhs.OrderNum > rhs.OrderNum;
	}

	UPROPERTY(EditAnywhere)
	int32 OrderNum = 0;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	UArrowComponent* ArrowComponent;

};
