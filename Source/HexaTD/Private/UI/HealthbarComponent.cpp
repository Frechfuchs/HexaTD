// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HealthbarComponent.h"
#include "UI/HealthbarWidget.h"

UHealthbarComponent::UHealthbarComponent()
{
    // Link to used HealthbarWidget
    static ConstructorHelpers::FClassFinder<UHealthbarWidget> HealthbarWidgetClass(TEXT("/Game/HexaTD/UI/Enemy/WBP_Healthbar"));
    SetWidgetClass(HealthbarWidgetClass.Class);
	SetWidgetSpace(EWidgetSpace::Screen);
	SetRelativeLocation(FVector(0.f, 0.f, 90.f));
	SetDrawSize(FVector2D(50.f, 10.f));
}

/**
 * @brief TODO
 * 
 * @param Percent 
 */
void UHealthbarComponent::SetHitpointsPercent(float Percent)
{
    // TODO: On Clients the BeginPlay get ref doesn't work. This should be investigated!
    if (!HealthbarWidget) HealthbarWidget = Cast<UHealthbarWidget>(GetWidget());
    if (HealthbarWidget)
    {
        HealthbarWidget->SetHitpointsPercent(Percent);
    }
}

/**
 * @brief TODO
 */
void UHealthbarComponent::BeginPlay()
{
    Super::BeginPlay();
    // Initialize Widget and get reference
    InitWidget();
    HealthbarWidget = Cast<UHealthbarWidget>(GetWidget());
}
