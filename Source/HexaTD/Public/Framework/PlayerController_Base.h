// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayerController_Base.generated.h"

class ABuilding_Base;
class AHexGrid;
class APlayerState_Base;
class UMaterialInstance;

/**
 * 
 */
UCLASS()
class HEXATD_API APlayerController_Base : public APlayerController
{
	GENERATED_BODY()

public:
	/** Called every frame */
	virtual void Tick(float DeltaTime) override;

	/** Set SelectedBuildingPreview and spawn it in preview mode */
	UFUNCTION(BlueprintCallable)
	void SetSelectedBuildingPreview(TSubclassOf<ABuilding_Base> BuildingClass, UMaterialInstance* MValid, UMaterialInstance* MInvalid);

	/**
	 * Replication
	 */
	/** TODO */
	UFUNCTION(Server, Reliable)
	void ServerSpawnBuilding(FVector Location, TSubclassOf<ABuilding_Base> BuildingClass);
	/** TODO */
	UFUNCTION(Server, Reliable)
	void ServerInitializePostLogin();
	/** TODO */
	UFUNCTION(Client, Reliable)
	void ClientInitializePostLogin();

protected:
	/** TODO */
	virtual void SetupInputComponent() override;

private:
	/** TODO */
	void InputClick();
	/** TODO */
	void GetCursorLocation(bool &IsValid, FVector &Location);
	/** TODO */
	bool HasEnoughResources(int32 Cost) const;

	/** TODO */
	TSubclassOf<ABuilding_Base> SelectedBuildingClass;
	/** TODO */
	ABuilding_Base* SelectedBuildingPreview;
	/** TODO */
	UMaterialInstance* MaterialValid;
	/** TODO */
	UMaterialInstance* MaterialInvalid;
	/** TODO */
	AHexGrid* HexGrid;
	/** TODO */
	APlayerState_Base* PlayerState;

	//TODO
	// Shows debugging UI
	//bool Debugging = false;
};
