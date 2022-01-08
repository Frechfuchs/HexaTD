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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Interval = 1.f;	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Damage = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsSlow = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float SlowPercent = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SlowTime = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsTeleport = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EffectTeleportType> TeleportType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsPoison = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PoisonDamage = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 PoisonTickCount = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PoisonInterval = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsPushBack = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
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
