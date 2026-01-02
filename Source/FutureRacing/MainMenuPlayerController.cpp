#include "MainMenuPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraActor.h"

void AMainMenuPlayerController::BeginPlay()
{
    Super::BeginPlay();


    // Show main menu widget (from property)
    if (MainMenuWidgetClass)
    {
        UUserWidget* MainMenu = CreateWidget<UUserWidget>(this, MainMenuWidgetClass);
        if (MainMenu)
        {
            MainMenu->AddToViewport();
            FInputModeUIOnly InputMode;
            InputMode.SetWidgetToFocus(MainMenu->TakeWidget());
            SetInputMode(InputMode);
            bShowMouseCursor = true;
        }
    }

    // Set camera view to menu camera
    SetMenuCamera();
}

void AMainMenuPlayerController::SetMenuCamera()
{
    if (MenuCameraActor)
    {
        SetViewTargetWithBlend(MenuCameraActor, 0.f);
    }
}
