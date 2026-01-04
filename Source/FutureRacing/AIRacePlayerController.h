#pragma once

#include "CoreMinimal.h"
#include "RacingPlayerController.h"
#include "AIRacePlayerController.generated.h"

UCLASS()
class FUTURERACING_API AAIRacePlayerController : public ARacingPlayerController
{
    GENERATED_BODY()

protected:

    // True if the race has started
    bool bRaceStarted = false;

    // Lap tracking
    int32 CurrentLap = 0;
    float LastLapStartTime = 0.0f;
    TArray<float> LapTimes;
    float RaceStartTime = 0.0f;

    // Type of UI widget to spawn
    UPROPERTY(EditAnywhere, Category="AI Race|UI")
    TSubclassOf<class UTimeTrialUI> UIWidgetClass;

    // Pointer to the UI Widget
    UPROPERTY()
    TObjectPtr<class UTimeTrialUI> UIWidget;

    // Type of the vehicle UI to spawn
    UPROPERTY(EditAnywhere, Category="Vehicle|UI")
    TSubclassOf<class UFutureRacingUI> VehicleUIClass;

    // Pointer to the vehicle UI widget
    UPROPERTY()
    TObjectPtr<class UFutureRacingUI> VehicleUI;


    // Type of win UI widget to spawn
    UPROPERTY(EditAnywhere, Category="AI Race|UI")
    TSubclassOf<UUserWidget> WinUIClass;

    // Type of lose UI widget to spawn
    UPROPERTY(EditAnywhere, Category="AI Race|UI")
    TSubclassOf<UUserWidget> LoseUIClass;

    // Pointer to the win UI widget
    UPROPERTY()
    TObjectPtr<UUserWidget> WinUI;

    // Pointer to the lose UI widget
    UPROPERTY()
    TObjectPtr<UUserWidget> LoseUI;

    // Override BeginPlay for debug
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;

    // Track if player finished before AI
    bool bPlayerWon = false;
    // Optionally, store AI finish time
    float AIFinishTime = 0.0f;

    // Start the race (can be called externally)
    virtual void StartRace() override;

    // Handle race finished
    virtual void HandleRaceFinished() override;

    // Lap increment logic
public:
    void IncrementLapCount();
    AActor* GetTargetGate() const { return TargetGate.Get(); }
    void SetTargetGate(AActor* Gate) { TargetGate = Gate; }
protected:
    // Target gate for lap tracking
    TObjectPtr<AActor> TargetGate;

public:
    // Called by AI when it finishes
    UFUNCTION(BlueprintCallable, Category="AI Race")
    void NotifyAIFinished(float FinishTime);

    UFUNCTION()
    void OnPawnDestroyed(AActor* DestroyedPawn);
};
