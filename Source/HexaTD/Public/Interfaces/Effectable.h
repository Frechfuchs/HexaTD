// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Effectable.generated.h"

USTRUCT(BlueprintType)
struct HEXATD_API FEffect
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float Damage = 1.f;
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
