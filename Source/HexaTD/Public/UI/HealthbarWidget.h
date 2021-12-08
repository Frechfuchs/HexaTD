// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthbarWidget.generated.h"

class UProgressBar;

/**
 * 
 */
UCLASS()
class HEXATD_API UHealthbarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
	void SetHitpointsPercent(float Percent);
};
