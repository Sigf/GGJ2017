// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Voice.h"
#include "OnlineSubsystemUtils.h"
#include "Sound/SoundWaveProcedural.h"

#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

UCLASS()
class GGJ2017_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(BlueprintReadOnly)
	float CurrentVolume;

	UPROPERTY(BlueprintReadOnly)
	int CurrentLevel;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	int LevelCounts;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	float MaxVolume;

	UFUNCTION(BlueprintCallable, Category = "Player Character")
	void GetCurrentVoiceLevel(int & Level);

private:

	UPROPERTY()
	USoundWaveProcedural* VoiceCaptureSoundsWaveProcedural;

	UPROPERTY()
	UAudioComponent* VoiceCaptureAudioComponent;

	TSharedPtr<class IVoiceCapture> VoiceCapture;

	TArray<uint8> VoiceCaptureBuffer;

	void VoiceCaptureTick();

	void SetCurrentLevel();
	
};
