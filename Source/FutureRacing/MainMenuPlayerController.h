#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MainMenuPlayerController.generated.h"

UCLASS()

class FUTURERACING_API AMainMenuPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    // Widget class to display as main menu
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Main Menu")
    TSubclassOf<class UUserWidget> MainMenuWidgetClass;

    // Camera actor to use for menu view
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Main Menu")
    class ACameraActor* MenuCameraActor = nullptr;

protected:
    virtual void BeginPlay() override;
    void SetMenuCamera();
};
