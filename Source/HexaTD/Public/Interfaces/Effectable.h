// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Effectable.generated.h"

UENUM(BlueprintType)
enum EffectTeleportType
{
  Spawn     	UMETA(DisplayName = "Spawn"),
  // TODO: Implement teleport to last reached checkpoint
  // Checkpoint    UMETA(DisplayName = "Checkpoint")
};

USTRUCT(BlueprintType)
struct HEXATD_API FEffect
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	float Damage = 1.f;

	UPROPERTY(BlueprintReadWrite)
	bool IsSlow = false;
	UPROPERTY(BlueprintReadWrite, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float SlowPercent = 0.f;
	UPROPERTY(BlueprintReadWrite)
	float SlowTime = 0.f;

	UPROPERTY(BlueprintReadWrite)
	bool IsTeleport = false;
	UPROPERTY(BlueprintReadWrite)
	TEnumAsByte<EffectTeleportType> TeleportType;

	UPROPERTY(BlueprintReadWrite)
	bool IsPoison = false;
	UPROPERTY(BlueprintReadWrite)
	float PoisonDamage = 0.f;
	UPROPERTY(BlueprintReadWrite)
	int32 PoisonTickCount = 0;
	UPROPERTY(BlueprintReadWrite)
	float PoisonInterval = 0.f;

	UPROPERTY(BlueprintReadWrite)
	bool IsPushBack = false;
	UPROPERTY(BlueprintReadWrite)
	float PushBackForce = 0.f;
};

/**
 * 
 */
UINTERFACE(BlueprintType)
class HEXATD_API UEffectable : public UInterface
{
	GENERATED_BODY()
	
};

class HEXATD_API IEffectable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Effect(FEffect Effect);
};
