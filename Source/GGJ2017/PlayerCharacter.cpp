// Fill out your copyright notice in the Description page of Project Settings.

#include "GGJ2017.h"
#include "PlayerCharacter.h"


// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	VoiceCaptureAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("VoiceCaptureAudioComponent"));
	VoiceCaptureAudioComponent->SetupAttachment(RootComponent);
	VoiceCaptureAudioComponent->bAutoActivate = true;
	VoiceCaptureAudioComponent->bAlwaysPlay = true;
	VoiceCaptureAudioComponent->PitchMultiplier = 0.85f;
	VoiceCaptureAudioComponent->VolumeMultiplier = 5.f;

	LevelCounts = 5;
	MaxVolume = 200;
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
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
void APlayerCharacter::Tick( float DeltaTime )
{
	Super::Tick(DeltaTime);

	VoiceCaptureTick();
	SetCurrentLevel();

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red, FString::Printf(TEXT("Current Level: %d"), CurrentLevel));
	}

}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void APlayerCharacter::GetCurrentVoiceLevel(int & Level)
{
	Level = CurrentLevel;
}

void APlayerCharacter::VoiceCaptureTick()
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
		float VoiceCaptureFinalVolume = ((VoiceCaptureRms / 32768.0) * 200.f);

		CurrentVolume = VoiceCaptureFinalVolume;

		VoiceCaptureAudioComponent->SetSound(VoiceCaptureSoundsWaveProcedural);
	}

	if (CaptureState == EVoiceCaptureState::NotCapturing)
	{
		CurrentVolume = 0.0;
	}
}

void APlayerCharacter::SetCurrentLevel()
{
	int Step = MaxVolume / LevelCounts;
	CurrentLevel = FMath::Ceil(CurrentVolume/Step);
}

