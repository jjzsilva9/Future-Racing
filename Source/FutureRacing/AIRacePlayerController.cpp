#include "AIRaceGameMode.h"
#include "FutureRacingUI.h"
#include "ChaosWheeledVehicleMovementComponent.h"
#include "AIRacePlayerController.h"
#include "FutureRacingPawn.h"
#include "Engine/World.h"
#include "TimeTrialUI.h"
#include "TimeTrialTrackGate.h"

#include "GameFramework/Pawn.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"

void AAIRacePlayerController::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Warning, TEXT("AAIRacePlayerController::BeginPlay called"));
    UE_LOG(LogTemp, Warning, TEXT("Joe Check: AAIRacePlayerController initialized and ticking!"));
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Joe Check: AAIRacePlayerController initialized and ticking!"));
    }
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("AAIRacePlayerController::BeginPlay called"));
    }
    if (GetPawn())
    {
        UE_LOG(LogTemp, Warning, TEXT("Pawn at BeginPlay: %s"), *GetPawn()->GetName());
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Pawn at BeginPlay: %s"), *GetPawn()->GetName()));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No pawn possessed at BeginPlay"));
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("No pawn possessed at BeginPlay"));
        }
    }
    // Only spawn UI on local player controllers
    if (IsLocalPlayerController())
    {
        // Spawn the main race UI (countdown, lap info, etc.)
        if (UIWidgetClass)
        {
            UIWidget = CreateWidget<UTimeTrialUI>(this, UIWidgetClass);
            if (UIWidget)
            {
                UIWidget->AddToViewport(0);
                UIWidget->OnRaceStart.AddDynamic(this, &AAIRacePlayerController::StartRace);
                UE_LOG(LogTemp, Warning, TEXT("AAIRacePlayerController: Bound OnRaceStart to StartRace."));
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("AAIRacePlayerController: Failed to create UIWidget!"));
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("AAIRacePlayerController: UIWidgetClass is not set! Countdown will not run."));
        }
        // Spawn the vehicle UI
        if (VehicleUIClass)
        {
            VehicleUI = CreateWidget<UFutureRacingUI>(this, VehicleUIClass);
            if (VehicleUI)
            {
                VehicleUI->AddToViewport(1);
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("AAIRacePlayerController: Failed to create VehicleUI widget!"));
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("AAIRacePlayerController: VehicleUIClass is not set! Vehicle UI will not display."));
        }
    }
}


void AAIRacePlayerController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    // Update vehicle UI with speed and gear
    if (IsValid(VehiclePawn) && IsValid(VehicleUI.Get()))
    {
        if (VehiclePawn->GetChaosVehicleMovement())
        {
            VehicleUI->UpdateSpeed(VehiclePawn->GetChaosVehicleMovement()->GetForwardSpeed());
            VehicleUI->UpdateGear(VehiclePawn->GetChaosVehicleMovement()->GetCurrentGear());
        }
    }
}



void AAIRacePlayerController::StartRace()
{
    UE_LOG(LogTemp, Warning, TEXT("AAIRacePlayerController: Ran StartRace."));

    // Set the initial target gate for AI race mode
    if (AAIRaceGameMode* GM = Cast<AAIRaceGameMode>(GetWorld()->GetAuthGameMode()))
    {
        SetTargetGate(GM->GetFinishLine()->GetNextMarker());
    }

    bRaceStarted = true;
    RaceStartTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;

    // Initialize lap timing
    LastLapStartTime = RaceStartTime;
    LapTimes.Empty();
    CurrentLap = 0;
    IncrementLapCount();

    Super::StartRace();
}
// Lap tracking variables

void AAIRacePlayerController::IncrementLapCount()
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

    // update the UI with total race time
    float TotalRaceTime = GetWorld()->GetTimeSeconds() - RaceStartTime;
    if (UIWidget)
    {
        UIWidget->UpdateLapCount(CurrentLap, GetWorld()->GetTimeSeconds(), TotalRaceTime);
    }

    // check if race is finished (get total laps from game mode)
    // For AI mode, you may want to use a custom game mode or hardcoded value
    int32 NumLaps = 3; // Default to 3 laps, or fetch from game mode if available
    // Example: if (AAIRaceGameMode* GM = Cast<AAIRaceGameMode>(GetWorld()->GetAuthGameMode())) { NumLaps = GM->GetLaps(); }
    if (CurrentLap > NumLaps)
    {
        HandleRaceFinished();
    }
}


void AAIRacePlayerController::HandleRaceFinished()
{
    float PlayerFinishTime = GetWorld() ? (GetWorld()->GetTimeSeconds() - RaceStartTime) : 0.0f;
    bPlayerWon = (AIFinishTime == 0.0f || PlayerFinishTime < AIFinishTime);
    Super::HandleRaceFinished();

    // Spawn and display WinUI or LoseUI based on result
    if (bPlayerWon)
    {
        if (WinUIClass)
        {
            WinUI = CreateWidget<UUserWidget>(this, WinUIClass);
            if (WinUI)
            {
                WinUI->AddToViewport(2);
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("AAIRacePlayerController: Failed to create WinUI widget!"));
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("AAIRacePlayerController: WinUIClass is not set! Win UI will not display."));
        }
    }
    else
    {
        if (LoseUIClass)
        {
            LoseUI = CreateWidget<UUserWidget>(this, LoseUIClass);
            if (LoseUI)
            {
                LoseUI->AddToViewport(2);
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("AAIRacePlayerController: Failed to create LoseUI widget!"));
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("AAIRacePlayerController: LoseUIClass is not set! Lose UI will not display."));
        }
    }
}


void AAIRacePlayerController::OnPawnDestroyed(AActor* DestroyedPawn)
{
    // find the player start
    TArray<AActor*> ActorList;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), ActorList);

    if (ActorList.Num() > 0 && VehiclePawn)
    {
        // spawn a vehicle at the player start using the current pawn's class
        const FTransform SpawnTransform = ActorList[0]->GetActorTransform();
        if (AFutureRacingPawn* RespawnedVehicle = GetWorld()->SpawnActor<AFutureRacingPawn>(VehiclePawn->GetClass(), SpawnTransform))
        {
            // possess the vehicle
            Possess(RespawnedVehicle);
        }
    }
}

void AAIRacePlayerController::NotifyAIFinished(float FinishTime)
{
    AIFinishTime = FinishTime;
}
