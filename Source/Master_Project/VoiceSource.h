// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "OnlineSubsystemUtils.h"

#include "VoiceDataCommon.h"
#include "Sound/SoundWaveProcedural.h"
#include "VoicePacketImpl.h"

#include "VoiceSource.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MASTER_PROJECT_API UVoiceSource : public UActorComponent
{
	GENERATED_BODY()

	/* Voice subsystem pointer */
	IOnlineVoicePtr m_voice;

	/* Timer handles for unregistering the timers, important for multiple sessions in one execution */
	FTimerHandle m_timerHandle;
	FTimerHandle m_timerFrequency;

	/* Mapping from player IDs to sound waves */
	TMap<FString, USoundWaveProcedural*> m_soundWaves;

public:	
	// Sets default values for this component's properties
	UVoiceSource();

	/* Exposed functions to BP */

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Voice Chat")
		USoundWave* GetPlayerSound(int id);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Voice Chat")
		float GetSoundDuration(USoundWave* wav);

	/* UE functions */

	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;
};
