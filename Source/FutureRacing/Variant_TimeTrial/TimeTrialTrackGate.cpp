// Copyright Epic Games, Inc. All Rights Reserved.


#include "TimeTrialTrackGate.h"
#include "Components/SceneComponent.h"
#include "Components/BoxComponent.h"
#include "TimeTrialPlayerController.h"
#include "AIRacePlayerController.h"

ATimeTrialTrackGate::ATimeTrialTrackGate()
{
 	PrimaryActorTick.bCanEverTick = true;

	// create the root component
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	// create the collision box
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision Box"));
	CollisionBox->SetupAttachment(RootComponent);

	CollisionBox->SetBoxExtent(FVector(1000.0f));
	CollisionBox->SetLineThickness(32.0f);
	CollisionBox->bHiddenInGame = false;
	CollisionBox->SetCollisionProfileName(FName("OverlapAllDynamic"));

}

void ATimeTrialTrackGate::NotifyActorBeginOverlap(AActor* OtherActor)
{
	// get the player controller of the overlapping actor
	AController* Controller = OtherActor->GetInstigatorController();
	if (ATimeTrialPlayerController* TTPC = Cast<ATimeTrialPlayerController>(Controller))
	{
		if (TTPC->GetTargetGate() == this)
		{
			TTPC->SetTargetGate(NextMarker);
			if (bIsFinishLine)
			{
				TTPC->IncrementLapCount();
			}
		}
	}
	else if (AAIRacePlayerController* AIRacePC = Cast<AAIRacePlayerController>(Controller))
	{
		if (AIRacePC->GetTargetGate() == this)
		{
			AIRacePC->SetTargetGate(NextMarker);
			if (bIsFinishLine)
			{
				AIRacePC->IncrementLapCount();
			}
		}
	}
}

ATimeTrialTrackGate* ATimeTrialTrackGate::GetNextMarker() const
{
	return NextMarker;
}
