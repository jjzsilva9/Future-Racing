// Copyright Epic Games, Inc. All Rights Reserved.


#include "TimeTrialPlayerController.h"
#include "TimeTrialUI.h"
#include "Engine/World.h"
#include "TimeTrialGameMode.h"
#include "TimeTrialTrackGate.h"
#include "TimeTrialSaveGame.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "FutureRacingUI.h"
#include "FutureRacingPawn.h"
#include "ChaosWheeledVehicleMovementComponent.h"
#include "Blueprint/UserWidget.h"
#include "FutureRacing.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "Widgets/Input/SVirtualJoystick.h"

void ATimeTrialPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// only spawn UI on local player controllers
	if (IsLocalPlayerController())
	{
		if (ShouldUseTouchControls())
		{
			// spawn the mobile controls widget
			MobileControlsWidget = CreateWidget<UUserWidget>(this, MobileControlsWidgetClass);

			if (MobileControlsWidget)
			{
				// add the controls to the player screen
				MobileControlsWidget->AddToPlayerScreen(0);

			} else {

				UE_LOG(LogFutureRacing, Error, TEXT("Could not spawn mobile controls widget."));

			}
		}

		// create the UI widget
		UIWidget = CreateWidget<UTimeTrialUI>(this, UIWidgetClass);

		if (UIWidget)
		{
			UIWidget->AddToViewport(0);

			// subscribe to the race start delegate
			UIWidget->OnRaceStart.AddDynamic(this, &ATimeTrialPlayerController::StartRace);

		} else {

			UE_LOG(LogFutureRacing, Error, TEXT("Could not spawn Time Trial UI widget."));

		}
		

		// spawn the UI widget and add it to the viewport
		VehicleUI = CreateWidget<UFutureRacingUI>(this, VehicleUIClass);

		if (VehicleUI)
		{
			VehicleUI->AddToViewport(0);

		} else {

			UE_LOG(LogFutureRacing, Error, TEXT("Could not spawn vehicle UI widget."));

		}

		// Bind to race finish event to show leaderboard
		OnRaceFinished.AddDynamic(this, &ATimeTrialPlayerController::ShowLeaderboard);
	}

}

void ATimeTrialPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// only add IMCs for local player controllers
	if (IsLocalPlayerController())
	{
		// Add Input Mapping Contexts
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
			{
				Subsystem->AddMappingContext(CurrentContext, 0);
			}

			// only add these IMCs if we're not using mobile touch input
			if (!ShouldUseTouchControls())
			{
				for (UInputMappingContext* CurrentContext : MobileExcludedMappingContexts)
				{
					Subsystem->AddMappingContext(CurrentContext, 0);
				}
			}
		}
	}
}

void ATimeTrialPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	SetViewTarget(InPawn);
	// get a pointer to the controlled pawn
	VehiclePawn = CastChecked<AFutureRacingPawn>(InPawn);

	// subscribe to the pawn's OnDestroyed delegate
	VehiclePawn->OnDestroyed.AddDynamic(this, &ATimeTrialPlayerController::OnPawnDestroyed);

	// disable input on the pawn if the race hasn't started yet
	if (!bRaceStarted)
	{
		VehiclePawn->DisableInput(this);
	}
}

void ATimeTrialPlayerController::Tick(float Delta)
{
	Super::Tick(Delta);

	if (IsValid(VehiclePawn) && IsValid(VehicleUI))
	{
		VehicleUI->UpdateSpeed(VehiclePawn->GetChaosVehicleMovement()->GetForwardSpeed());
		VehicleUI->UpdateGear(VehiclePawn->GetChaosVehicleMovement()->GetCurrentGear());
	}

	// Interval-based ghost recording: record at set intervals with timestamp
	if (IsValid(VehiclePawn) && bRaceStarted)
	{
		GhostRecordAccumulator += Delta;
		if (GhostRecordAccumulator >= GhostRecordInterval)
		{
			FGhostFrame Frame;
			Frame.Time = GetWorld()->GetTimeSeconds() - RaceStartTime;
			Frame.Transform = VehiclePawn->GetActorTransform();
			RecordedGhostFrames.Add(Frame);
			GhostRecordAccumulator = 0.0f;
		}
	}
}

void ATimeTrialPlayerController::StartRace()
{
	// get the finish line from the game mode
	if (ATimeTrialGameMode* GM = Cast<ATimeTrialGameMode>(GetWorld()->GetAuthGameMode()))
	{
		SetTargetGate(GM->GetFinishLine()->GetNextMarker());
	}

	// raise the race started flag so any respawned vehicles start with controls unlocked 
	bRaceStarted = true;

	// initialize race timing
	RaceStartTime = GetWorld()->GetTimeSeconds();
	LastLapStartTime = RaceStartTime;
	LapTimes.Empty();

	// start the first lap
	CurrentLap = 0;
	IncrementLapCount();

	// enable input on the pawn
	GetPawn()->EnableInput(this);

	// Start ghost recording (always record)
	RecordedGhostFrames.Empty();
	GhostRecordAccumulator = 0.0f;

	// Spawn ghost car if a best ghost exists
	if (HasAuthority())
	{
		const FString GhostSlot = TEXT("TimeTrialGhost");
		if (UGameplayStatics::DoesSaveGameExist(GhostSlot, 0))
		{
			UTimeTrialGhostSaveGame* GhostSave = Cast<UTimeTrialGhostSaveGame>(UGameplayStatics::LoadGameFromSlot(GhostSlot, 0));
			if (GhostSave && GhostSave->GhostFrames.Num() > 1 && GhostCarClass)
			{
				// Find player start
				TArray<AActor*> Starts;
				UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), Starts);
				FTransform SpawnTransform = Starts.Num() > 0 ? Starts[0]->GetActorTransform() : FTransform::Identity;
				// Spawn ghost car using the Blueprint class
				GhostCar = GetWorld()->SpawnActor<ATimeTrialGhostCar>(GhostCarClass, SpawnTransform);
				if (GhostCar)
				{
					UE_LOG(LogTemp, Warning, TEXT("Ghost car spawned!"));
					if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Ghost car spawned!"));
					GhostCar->InitializeGhost(GhostSave->GhostFrames);
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("Failed to spawn ghost car!"));
					if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Failed to spawn ghost car!"));
				}
			}
		}
	}
}

void ATimeTrialPlayerController::IncrementLapCount()
{
	const float CurrentTime = GetWorld()->GetTimeSeconds();
	const float LapTime = CurrentTime - LastLapStartTime;

	// store lap time (skip first increment since that's lap 0 -> 1)
	if (CurrentLap > 0)
	{
		LapTimes.Add(LapTime);
	}

	LastLapStartTime = CurrentTime;

	// increment the lap counter
	++CurrentLap;

	// update the UI
	UIWidget->UpdateLapCount(CurrentLap, GetWorld()->GetTimeSeconds());

	// check if race is finished (get total laps from game mode)
	if (ATimeTrialGameMode* GM = Cast<ATimeTrialGameMode>(GetWorld()->GetAuthGameMode()))
	{
		if (CurrentLap > GM->GetLaps())
		{
			HandleRaceFinished();
		}
	}
}

void ATimeTrialPlayerController::HandleRaceFinished()
{
	const float TotalTime = GetWorld()->GetTimeSeconds() - RaceStartTime;

	// find best lap time
	float BestLap = FLT_MAX;
	for (float Lap : LapTimes)
	{
		if (Lap < BestLap)
		{
			BestLap = Lap;
		}
	}

	// save to leaderboard
	SaveLeaderboardTime(TotalTime, BestLap);

	// disable input
	if (VehiclePawn)
	{
		VehiclePawn->DisableInput(this);
	}


	// Save ghost if new best time
	SaveGhostIfBest(TotalTime);

	// broadcast event for UI or other systems to handle
	OnRaceFinished.Broadcast(TotalTime, BestLap);

}

void ATimeTrialPlayerController::SaveGhostIfBest(float TotalTime)
{
	// Load existing ghost save
	const FString GhostSlot = TEXT("TimeTrialGhost");
	UTimeTrialGhostSaveGame* GhostSave = nullptr;
	if (UGameplayStatics::DoesSaveGameExist(GhostSlot, 0))
	{
		GhostSave = Cast<UTimeTrialGhostSaveGame>(UGameplayStatics::LoadGameFromSlot(GhostSlot, 0));
	}
	if (!GhostSave)
	{
		GhostSave = Cast<UTimeTrialGhostSaveGame>(UGameplayStatics::CreateSaveGameObject(UTimeTrialGhostSaveGame::StaticClass()));
	}

	// Save if no previous best or this run is better
	if (!GhostSave->BestTime || TotalTime < GhostSave->BestTime)
	{
		GhostSave->BestTime = TotalTime;
		GhostSave->GhostFrames = RecordedGhostFrames;
		UGameplayStatics::SaveGameToSlot(GhostSave, GhostSlot, 0);
	}
}

void ATimeTrialPlayerController::SaveLeaderboardTime(float TotalTime, float BestLap)
{
	UTimeTrialSaveGame* SaveGame = LoadOrCreateSaveGame();

	FString DefaultPlayerName = TEXT("Player1"); // Set your default name here

	if (SaveGame && SaveGame->IsQualifyingTime(TotalTime))
	{
		FTimeTrialRecord NewRecord;
		NewRecord.PlayerName = DefaultPlayerName; // Use default name
		NewRecord.TotalTime = TotalTime;
		NewRecord.BestLapTime = BestLap;
		NewRecord.RecordDate = FDateTime::Now();

		bool MadeTopTen = SaveGame->AddTime(NewRecord);

		// save to disk
		UGameplayStatics::SaveGameToSlot(SaveGame, SaveSlotName, 0);

		// broadcast event for UI or other systems to handle
		OnNewLeaderboardRecord.Broadcast(MadeTopTen);
	}
}

UTimeTrialSaveGame* ATimeTrialPlayerController::LoadOrCreateSaveGame()
{
	UTimeTrialSaveGame* SaveGame = nullptr;

	if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, 0))
	{
		SaveGame = Cast<UTimeTrialSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0));
	}

	if (!SaveGame)
	{
		SaveGame = Cast<UTimeTrialSaveGame>(UGameplayStatics::CreateSaveGameObject(UTimeTrialSaveGame::StaticClass()));
	}

	return SaveGame;
}

ATimeTrialTrackGate* ATimeTrialPlayerController::GetTargetGate()
{
	return TargetGate.Get();
}

void ATimeTrialPlayerController::SetTargetGate(ATimeTrialTrackGate* Gate)
{
	TargetGate = Gate;
}

void ATimeTrialPlayerController::OnPawnDestroyed(AActor* DestroyedPawn)
{
	// find the player start
	TArray<AActor*> ActorList;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), ActorList);

	if (ActorList.Num() > 0)
	{
		// spawn a vehicle at the player start
		const FTransform SpawnTransform = ActorList[0]->GetActorTransform();

		if (AFutureRacingPawn* RespawnedVehicle = GetWorld()->SpawnActor<AFutureRacingPawn>(VehiclePawnClass, SpawnTransform))
		{
			// possess the vehicle
			Possess(RespawnedVehicle);
		}
	}
}

bool ATimeTrialPlayerController::ShouldUseTouchControls() const
{
	// are we on a mobile platform? Should we force touch?
	return SVirtualJoystick::ShouldDisplayTouchInterface() || bForceTouchControls;
}

void ATimeTrialPlayerController::ShowLeaderboard(float TotalTime, float BestLapTime)
{
	if (LeaderboardWidgetClass)
	{
		UUserWidget* LeaderboardWidget = CreateWidget<UUserWidget>(this, LeaderboardWidgetClass);
		if (LeaderboardWidget)
		{
			LeaderboardWidget->AddToViewport();
		}
	}
}
