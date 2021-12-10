// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayerController_Base.generated.h"

class ABuilding_Base;
class AGameMode_Base;
class AHexGrid;
class APlayerState_Base;
class UMaterialInstance;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDelegate_NotifyPlayerControllerChange);

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

	/**
	 * Gameplay
	 */
	/** Set SelectedBuildingPreview and spawn it in preview mode */
	UFUNCTION(BlueprintCallable)
	void SetSelectedBuildingPreview(TSubclassOf<ABuilding_Base> BuildingClass, UMaterialInstance* MValid, UMaterialInstance* MInvalid);
	/** TODO */
	UFUNCTION(BlueprintCallable)
	void DestroySelectedBuilding();
	/** TODO */
	UFUNCTION(BlueprintCallable)
	void UpgradeSelectedBuilding();	

	/**
	 * Replication
	 */
	/** TODO */
	UFUNCTION(Server, Reliable)
	void ServerSpawnBuilding(FVector Location, TSubclassOf<ABuilding_Base> BuildingClass);
	/** TODO */
	UFUNCTION(Server, Reliable)
	void ServerDestroyBuilding(ABuilding_Base* Building);
	/** TODO */
	UFUNCTION(Server, Reliable)
	void ServerUpgradeBuilding(ABuilding_Base* Building);
	/** TODO */
	UFUNCTION(Client, Reliable)
	void ClientPostUpgradeBuilding();
	/** TODO */
	UFUNCTION(Server, Reliable)
	void ServerInitializePostLogin();
	/** TODO */
	UFUNCTION(Client, Reliable)
	void ClientInitializePostLogin();

	/**
	 * Getters & Setters
	 */
	/** TODO */
	UFUNCTION(BlueprintCallable)
	ABuilding_Base* GetSelectedBuilding() const;

	/**
	 * Delegates
	 */
	/** TODO */
	UPROPERTY(BlueprintAssignable)
	FDelegate_NotifyPlayerControllerChange BuildingSelected;

protected:
	/** TODO */
	virtual void SetupInputComponent() override;

private:
	/** TODO */
	void InputClick();
	/** TODO */
	void InputRightClick();
	/** TODO */
	void GetCursorLocation(bool &IsValid, FVector &Location);
	/** TODO */
	bool HasEnoughResources(int32 Cost) const;

	/** TODO */
	TSubclassOf<ABuilding_Base> SelectedBuildingClass;
	/** TODO */
	ABuilding_Base* SelectedBuilding;
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
	/** TODO */
	AGameMode_Base* GameMode;
};
