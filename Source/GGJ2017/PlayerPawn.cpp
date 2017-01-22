// Fill out your copyright notice in the Description page of Project Settings.

#include "GGJ2017.h"
#include "PlayerPawn.h"


// Sets default values
APlayerPawn::APlayerPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	VoiceCaptureAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("VoiceCaptureAudioComponent"));
	VoiceCaptureAudioComponent->SetupAttachment(RootComponent);
	VoiceCaptureAudioComponent->bAutoActivate = true;
	VoiceCaptureAudioComponent->bAlwaysPlay = true;
	VoiceCaptureAudioComponent->PitchMultiplier = 0.85f;
	VoiceCaptureAudioComponent->VolumeMultiplier = 5.f;

	MaxHeight = 1000;
	MaxVolume = 200;
	LevelCounts = 5;
	VerticalSpeed = 5;
	HorizontalSpeed = 2;
	CurrentHeight = 0;
	VolumeMultiplier = 1;
}

// Called when the game starts or when spawned
void APlayerPawn::BeginPlay()
{
	Super::BeginPlay();
	VoiceCapture = FVoiceModule::Get().CreateVoiceCapture();

	VoiceCaptureSoundsWaveProcedural = NewObject<USoundWaveProcedural>();
	VoiceCaptureSoundsWaveProcedural->SampleRate = 16000;
	VoiceCaptureSoundsWaveProcedural->NumChannels = 1;
	VoiceCaptureSoundsWaveProcedural->Duration = INDEFINITELY_LOOPING_DURATION;
	VoiceCaptureSoundsWaveProcedural->SoundGroup = SOUNDGROUP_Voice;
	VoiceCaptureSoundsWaveProcedural->bLooping = false;
	VoiceCaptureSoundsWaveProcedural->bProcedural = true;

	if (VoiceCapture.IsValid())
	{
		VoiceCapture->Start();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("VoiceController not valid."));
	}
	
}

// Called every frame
void APlayerPawn::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	VoiceCaptureTick();
	SetCurrentLevel();
	MoveToLevel(DeltaTime);
	MoveRight(DeltaTime);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red, FString::Printf(TEXT("Current Level: %d"), CurrentLevel));
	}
}

// Called to bind functionality to input
void APlayerPawn::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

}

void APlayerPawn::VoiceCaptureTick()
{
	if (!VoiceCapture.IsValid() || VoiceCaptureSoundsWaveProcedural == nullptr)
	{
		return;
	}

	uint32 VoiceCaptureBytesAvailable = 0;
	EVoiceCaptureState::Type CaptureState = VoiceCapture->GetCaptureState(VoiceCaptureBytesAvailable);

	VoiceCaptureBuffer.Reset();

	if (CaptureState == EVoiceCaptureState::Ok && VoiceCaptureBytesAvailable > 0)
	{
		int16_t VoiceCaptureSample;
		uint32 VoiceCaptureReadBytes;
		float VoiceCaptureTotalSquared = 0;

		VoiceCaptureBuffer.SetNumUninitialized(VoiceCaptureBytesAvailable);

		VoiceCapture->GetVoiceData(VoiceCaptureBuffer.GetData(), VoiceCaptureBytesAvailable, VoiceCaptureReadBytes);

		for (uint32 i = 0; i < (VoiceCaptureReadBytes / 2); i++)
		{
			VoiceCaptureSample = (VoiceCaptureBuffer[i * 2 + 1] << 8) | VoiceCaptureBuffer[i * 2];
			VoiceCaptureTotalSquared += ((float)VoiceCaptureSample * (float)VoiceCaptureSample);

		}

		VoiceCaptureSoundsWaveProcedural->QueueAudio(VoiceCaptureBuffer.GetData(), VoiceCaptureReadBytes);

		float VoiceCaptureMeanSquare = (2 * (VoiceCaptureTotalSquared / VoiceCaptureBuffer.Num()));
		float VoiceCaptureRms = FMath::Sqrt(VoiceCaptureMeanSquare);
		float VoiceCaptureFinalVolume = ((VoiceCaptureRms / 32768.0) * 200.f * VolumeMultiplier);

		CurrentVolume = VoiceCaptureFinalVolume;
		if (CurrentVolume > MaxVolume)
		{
			CurrentVolume = MaxVolume;
		}

		VoiceCaptureAudioComponent->SetSound(VoiceCaptureSoundsWaveProcedural);
	}

	if (CaptureState == EVoiceCaptureState::NotCapturing)
	{
		CurrentVolume = 0.0;
	}
}

void APlayerPawn::SetCurrentLevel()
{
	int Step = MaxVolume / LevelCounts;
	CurrentLevel = FMath::Ceil(CurrentVolume / Step);
}

void APlayerPawn::MoveToLevel(float DeltaTime)
{

	float NewHeight = (MaxHeight / LevelCounts) * CurrentLevel;
	float Alpha = VerticalSpeed * DeltaTime;

	if (Alpha > 1)
	{
		Alpha = 1;
	}

	CurrentHeight = FMath::Lerp(CurrentHeight, NewHeight, Alpha);

	//Mesh->SetRelativeLocation(NewLocation, true);
}

void APlayerPawn::MoveRight(float DeltaTime)
{
	AddActorWorldOffset(FVector(0.0, DeltaTime * HorizontalSpeed, 0.0), true);
}

