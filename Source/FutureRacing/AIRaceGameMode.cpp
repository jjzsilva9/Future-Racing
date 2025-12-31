#include "AIRaceGameMode.h"
#include "AIRacePlayerController.h"

AAIRaceGameMode::AAIRaceGameMode()
{
    PlayerControllerClass = AAIRacePlayerController::StaticClass();
    // Optionally set DefaultPawnClass, HUDClass, etc. here if needed
}
