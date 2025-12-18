#include "TimeTrialGhostCar.h"
#include "Components/StaticMeshComponent.h"

ATimeTrialGhostCar::ATimeTrialGhostCar()
{
	PrimaryActorTick.bCanEverTick = true;

	// Optionally add a mesh for visualization (replace with your ghost mesh)
	UStaticMeshComponent* Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GhostMesh"));
	RootComponent = Mesh;
	// You can set material/translucency in Blueprint or here
}

void ATimeTrialGhostCar::BeginPlay()
{
	Super::BeginPlay();
	PlaybackTime = 0.0f;
	CurrentFrameIndex = 0;
}

void ATimeTrialGhostCar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (GhostFrames.Num() < 2) return;

	PlaybackTime += DeltaTime;
	UpdateGhostTransform();
}

void ATimeTrialGhostCar::InitializeGhost(const TArray<FGhostFrame>& InFrames)
{
	GhostFrames = InFrames;
	PlaybackTime = 0.0f;
	CurrentFrameIndex = 0;
	if (GhostFrames.Num() > 0)
	{
		SetActorTransform(GhostFrames[0].Transform);
	}
}

void ATimeTrialGhostCar::UpdateGhostTransform()
{
	if (CurrentFrameIndex >= GhostFrames.Num() - 1) return;

	// Advance to the correct frame
	while (CurrentFrameIndex < GhostFrames.Num() - 2 && PlaybackTime > GhostFrames[CurrentFrameIndex + 1].Time)
	{
		++CurrentFrameIndex;
	}

	const FGhostFrame& FrameA = GhostFrames[CurrentFrameIndex];
	const FGhostFrame& FrameB = GhostFrames[CurrentFrameIndex + 1];
	float FrameDelta = FrameB.Time - FrameA.Time;
	float Alpha = (FrameDelta > 0) ? (PlaybackTime - FrameA.Time) / FrameDelta : 0.0f;
	FTransform InterpTransform = FTransform::Identity;
	InterpTransform.Blend(FrameA.Transform, FrameB.Transform, Alpha);
	SetActorTransform(InterpTransform);
}
