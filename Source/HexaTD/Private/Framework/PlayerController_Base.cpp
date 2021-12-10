// Fill out your copyright notice in the Description page of Project Settings.

#include "Framework/PlayerController_Base.h"
#include "Framework/Building_Base.h"
#include "Framework/GameMode_Base.h"
#include "Framework/PlayerPawn_Base.h"
#include "Framework/PlayerState_Base.h"
#include "HexGrid/HexGrid.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstance.h"

/**
 * @brief Called every frame
 * 
 * @param DeltaTime 
 */
void APlayerController_Base::Tick(float DeltaTime)
{
    // Check for a selected building
    if (SelectedBuildingPreview)
    {
        bool IsValidPosition;
        FVector NewLocation;
        GetCursorLocation(IsValidPosition, NewLocation);
        if (IsValidPosition && PlayerState->GetAllowBuilding() && HasEnoughResources(SelectedBuildingPreview->ResourceCost))
        {
            SelectedBuildingPreview->SetMeshMaterial(MaterialValid);
        }
        else
        {
            SelectedBuildingPreview->SetMeshMaterial(MaterialInvalid);
        }
        
        // Teleport preview to new location
        SelectedBuildingPreview->SetActorLocation(NewLocation, false, nullptr, ETeleportType::TeleportPhysics);
    }
}

/**
 * @brief 
 * 
 * @param BuildingClass 
 * @param MValid 
 * @param MInvalid 
 */
void APlayerController_Base::SetSelectedBuildingPreview(TSubclassOf<ABuilding_Base> BuildingClass, UMaterialInstance* MValid, UMaterialInstance* MInvalid)
{
    // TODO: Do we need a ClassProperty here?
    SelectedBuildingClass = BuildingClass;
    // TODO: Link Material directly?
    MaterialValid = MValid;
    MaterialInvalid = MInvalid;

    if (!SelectedBuildingPreview)
    {
        // Spawn Actor as deferred, so we can set props before it gets initiliazed.
        /* SelectedBuildingPreview = GetWorld()->SpawnActorDeferred<ABuilding_Base>(SelectedBuildingClass, FTransform()); */

        SelectedBuildingPreview = Cast<ABuilding_Base>(UGameplayStatics::BeginDeferredActorSpawnFromClass(
            GetWorld(), SelectedBuildingClass, FTransform(), ESpawnActorCollisionHandlingMethod::AlwaysSpawn, this));
        if (SelectedBuildingPreview)
        {
            // Set is Preview
            SelectedBuildingPreview->bIsPreview = true;
            UGameplayStatics::FinishSpawningActor(SelectedBuildingPreview, FTransform());
        }
    }
}

/**
 * @brief TODO
 * 
 */
void APlayerController_Base::DestroySelectedBuilding()
{
    if (SelectedBuilding)
    {
        ServerDestroyBuilding(SelectedBuilding);
        SelectedBuilding = nullptr;
        BuildingSelected.Broadcast();
    }
}

/**
 * @brief TODO
 * 
 */
void APlayerController_Base::UpgradeSelectedBuilding()
{
    //UE_LOG(LogTemp, Warning, TEXT("UpgradeCost: %d"), SelectedBuilding->GetUpgradeCost());
    if (SelectedBuilding && HasEnoughResources(SelectedBuilding->GetUpgradeCost()))
    {
        ServerUpgradeBuilding(SelectedBuilding);
    }
}

// TODO: Use CubeCoords instead of Location
/**
 * @brief Spawn a selected building on the server
 * 
 * @param Location 
 */
void APlayerController_Base::ServerSpawnBuilding_Implementation(FVector Location, TSubclassOf<ABuilding_Base> BuildingClass)
{
    // TODO: Check if valid to spawn (ressources)!
    int32 ResourceCost = BuildingClass.GetDefaultObject()->ResourceCost;
    if (HexGrid && GameMode->IsBuildingPhase() && HasEnoughResources(ResourceCost))
    {
        bool Success;
        HexGrid->OccupieGridSpace(Location, Success);

        if (Success)
        {
            FTransform SpawnTransform = FTransform(FQuat(0.f, 0.f, 0.f, 0.f), Location, FVector(1.f));
            // Spawn Actor as deferred, so we can set props before it gets initiliazed.
            ABuilding_Base* Building = Cast<ABuilding_Base>(UGameplayStatics::BeginDeferredActorSpawnFromClass(
                GetWorld(), BuildingClass, SpawnTransform, ESpawnActorCollisionHandlingMethod::AlwaysSpawn, this));
            if (Building)
            {
                // Set Props here
                Building->SetIsPreview(false);
                UGameplayStatics::FinishSpawningActor(Building, SpawnTransform);
                PlayerState->ResourceAmount -= ResourceCost;
                // Call OnRep for host
                // TODO: Maybe there is a better solution
                PlayerState->OnRep_ResourceAmountUpdated();
            }
        }
    }
}

/**
 * @brief TODO
 * 
 * @param Building 
 */
void APlayerController_Base::ServerDestroyBuilding_Implementation(ABuilding_Base* Building)
{
    if (Building) Building->Destroy();
}

/**
 * @brief TODO
 * 
 * @param Building 
 */
void APlayerController_Base::ServerUpgradeBuilding_Implementation(ABuilding_Base* Building)
{
    if (Building)
    {
        int32 UpgradeCost = Building->GetUpgradeCost();
        if (HasEnoughResources(UpgradeCost))
        {
            PlayerState->ResourceAmount -= UpgradeCost;
            PlayerState->OnRep_ResourceAmountUpdated();
            Building->Upgrade();
            // Broadcast for host, client will be notified via OnRep_Level in Building
            BuildingSelected.Broadcast();
        }
    }
}

/**
 * @brief TODO
 * 
 * @param Building 
 */
void APlayerController_Base::ClientPostUpgradeBuilding_Implementation()
{
    BuildingSelected.Broadcast();
}

/**
 * @brief Being called after player logs in.
 * 
 */
void APlayerController_Base::ServerInitializePostLogin_Implementation()
{
    // Get references
    PlayerState = GetPlayerState<APlayerState_Base>();
    HexGrid = Cast<AHexGrid>(UGameplayStatics::GetActorOfClass(GetWorld(), AHexGrid::StaticClass()));
    GameMode = GetWorld()->GetAuthGameMode<AGameMode_Base>();

    // Wait for the PlayerState to initialize before the remote PlayerController tries to find it
    FTimerHandle TimerHandle;
    float Delay = 1.f;
    GetWorldTimerManager().SetTimer(TimerHandle, this, &APlayerController_Base::ClientInitializePostLogin, Delay, false);
    
    // Error Handle
    if (!HexGrid) UE_LOG(LogTemp, Error, TEXT("No HexGrid found!"));
}

/**
 * @brief Called by ServerInitializePostLogin_Implementation.
 * It will initialize HUD and remote PlayerController.
 * 
 */
void APlayerController_Base::ClientInitializePostLogin_Implementation()
{
    // Get references
    PlayerState = GetPlayerState<APlayerState_Base>();
    HexGrid = Cast<AHexGrid>(UGameplayStatics::GetActorOfClass(GetWorld(), AHexGrid::StaticClass()));

    // Show Cursor for Client
    SetShowMouseCursor(true);
    // Enable Tick
    SetActorTickEnabled(true);

    // ErrorHandling
    if (!HexGrid) UE_LOG(LogTemp, Error, TEXT("No HexGrid found!"));
    if (!PlayerState) UE_LOG(LogTemp, Error, TEXT("No PlayerState found!"));
}

/**
 * @brief 
 * 
 * @return ABuilding_Base* 
 */
ABuilding_Base* APlayerController_Base::GetSelectedBuilding() const
{
    return SelectedBuilding;
}

/**
 * @brief Setup InputComponents
 * 
 */
void APlayerController_Base::SetupInputComponent()
{
    Super::SetupInputComponent();

    InputComponent->BindAction("Click", IE_Pressed, this, &APlayerController_Base::InputClick);
    InputComponent->BindAction("RightClick", IE_Pressed, this, &APlayerController_Base::InputRightClick);
}

/**
 * @brief Handle clicking
 * 
 */
void APlayerController_Base::InputClick()
{
    // When SelectedBuildingPreview is active, try building
    if (SelectedBuildingPreview && PlayerState && PlayerState->GetAllowBuilding() 
         && HasEnoughResources(SelectedBuildingPreview->ResourceCost))
    {
        bool IsValidPosition;
        FVector Location;
        GetCursorLocation(IsValidPosition, Location);

        if (IsValidPosition && SelectedBuildingClass)
        {
            ServerSpawnBuilding(Location, SelectedBuildingClass);
        }
    }
    // When no SelectedBuildingPreview present, try to find an Actor
    else
    {
        FHitResult HitResult; 
        GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, HitResult);
        AActor* Actor = HitResult.GetActor();
        if (Actor)
        {
            // When another building was selected, unselect old one
            // TODO: Refactor to UnSelectBuilding() for reuse
            if (SelectedBuilding)
            {
                SelectedBuilding->SetDecalVisibility(false);
                // Unbind from selected building delegate
                if (SelectedBuilding->OnLevelChanged.IsBound())
                    SelectedBuilding->OnLevelChanged.RemoveDynamic(this, &APlayerController_Base::ClientPostUpgradeBuilding);
            }
            // Check for Building
            if (Actor->IsA(ABuilding_Base::StaticClass()))
            {
 
                SelectedBuilding = Cast<ABuilding_Base>(Actor);
                SelectedBuilding->SetDecalVisibility(true);
                // bind for level upgrade notification
                SelectedBuilding->OnLevelChanged.AddDynamic(this, &APlayerController_Base::ClientPostUpgradeBuilding);
                BuildingSelected.Broadcast();
            }
            else
            {
                SelectedBuilding = nullptr;
                BuildingSelected.Broadcast();
            }
        }
        
    }
}

/**
 * @brief Handle right-clicking
 * 
 */
void APlayerController_Base::InputRightClick()
{
    if (SelectedBuildingPreview)
    {
        SelectedBuildingPreview->Destroy();
        SelectedBuildingPreview = nullptr;
    }
}

/**
 * @brief 
 * 
 * @param IsValid 
 * @param Location 
 */
void APlayerController_Base::GetCursorLocation(bool &IsValid, FVector &Location)
{
    if (!HexGrid)
    {
        // Early return if no HexGrid is available
        IsValid = false;
        Location = FVector(0.f);
        return;
    }

    FHitResult HitResult; 
    GetHitResultUnderCursor(ECollisionChannel::ECC_PhysicsBody, true, HitResult);
    Location = HexGrid->SnapToGrid(HitResult.Location, IsValid);
}

bool APlayerController_Base::HasEnoughResources(int32 Cost) const
{
    return PlayerState->ResourceAmount >= Cost;
}
