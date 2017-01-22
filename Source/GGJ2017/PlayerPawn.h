// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Voice.h"
#include "OnlineSubsystemUtils.h"
#include "Sound/SoundWaveProcedural.h"

#include "GameFramework/Pawn.h"
#include "PlayerPawn.generated.h"



UCLASS()
class GGJ2017_API APlayerPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APlayerPawn();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	UPROPERTY(BlueprintReadOnly)
	float CurrentVolume;

	UPROPERTY(BlueprintReadOnly)
	int CurrentLevel;

	UPROPERTY(BlueprintReadOnly)
	float CurrentHeight;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	int LevelCounts;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	float MaxVolume;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	float MaxHeight;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	float VerticalSpeed;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	float HorizontalSpeed;


private:

	UPROPERTY()
	USoundWaveProcedural* VoiceCaptureSoundsWaveProcedural;

	UPROPERTY()
	UAudioComponent* VoiceCaptureAudioComponent;

	TSharedPtr<class IVoiceCapture> VoiceCapture;

	TArray<uint8> VoiceCaptureBuffer;

	void VoiceCaptureTick();

	void SetCurrentLevel();

	void MoveToLevel(float DeltaTime);
	
	void MoveRight(float DeltaTime);
};
