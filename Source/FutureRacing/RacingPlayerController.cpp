#include "RacingPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "FutureRacingPawn.h"

void ARacingPlayerController::BeginPlay()
{
	Super::BeginPlay();
	if (GetPawn())
	{
		GetPawn()->DisableInput(this);
	}
}

void ARacingPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (IsLocalPlayerController())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
			{
				Subsystem->AddMappingContext(CurrentContext, 0);
			}
			if (!MobileExcludedMappingContexts.IsEmpty())
			{
				for (UInputMappingContext* CurrentContext : MobileExcludedMappingContexts)
				{
					Subsystem->AddMappingContext(CurrentContext, 0);
				}
			}
		}
	}
}

void ARacingPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	VehiclePawn = Cast<AFutureRacingPawn>(InPawn);
	if (VehiclePawn)
	{
		VehiclePawn->DisableInput(this);
	}
}

void ARacingPlayerController::StartRace()
{
	UE_LOG(LogTemp, Warning, TEXT("ARacingPlayerController::StartRace called. Enabling input."));
	if (GetPawn())
	{
		GetPawn()->EnableInput(this);
	}
}

void ARacingPlayerController::HandleRaceFinished()
{
	if (VehiclePawn)
	{
		VehiclePawn->DisableInput(this);
	}
}
