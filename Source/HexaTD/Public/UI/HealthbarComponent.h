// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "HealthbarComponent.generated.h"

class UHealthbarWidget;

/**
 * 
 */
UCLASS()
class HEXATD_API UHealthbarComponent : public UWidgetComponent
{
	GENERATED_BODY()

	UHealthbarComponent();

public:
	UFUNCTION()
	void SetHitpointsPercent(float Percent);

	UHealthbarWidget* HealthbarWidget;

protected:
	void BeginPlay() override;
};
