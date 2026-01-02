#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputMappingContext.h"
#include "RacingPlayerController.generated.h"

class AFutureRacingPawn;

/**
 * Base PlayerController for all racing modes.
 * Handles input setup, mapping context, and shared vehicle/player logic.
 */
UCLASS(Abstract)
class FUTURERACING_API ARacingPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	// Input Mapping Contexts
	UPROPERTY(EditAnywhere, Category = "Input|Input Mappings")
	TArray<UInputMappingContext*> DefaultMappingContexts;

	UPROPERTY(EditAnywhere, Category = "Input|Input Mappings")
	TArray<UInputMappingContext*> MobileExcludedMappingContexts;

	// Pointer to the controlled pawn
	UPROPERTY()
	AFutureRacingPawn* VehiclePawn = nullptr;

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void OnPossess(APawn* InPawn) override;

	// Input enable/disable logic for race start/finish
	UFUNCTION()
	virtual void StartRace();
	virtual void HandleRaceFinished();
};
