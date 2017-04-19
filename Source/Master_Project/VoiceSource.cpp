// Fill out your copyright notice in the Description page of Project Settings.

#include "Master_Project.h"

#include "VoiceSource.h"

// Sets default values for this component's properties
UVoiceSource::UVoiceSource()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UVoiceSource::BeginPlay()
{
	Super::BeginPlay();

	/* First, get the Steam subsystem, as this provides voice services */
	auto online = IOnlineSubsystem::Get("Steam");
	if (!online)
	{
		UE_LOG(LogGarbage, Warning, TEXT("Failed to online"));
		return;
	}

	/* Get the voice subsystem from Steam */
	auto voice = online->GetVoiceInterface();
	if (!voice.IsValid())
	{
		UE_LOG(LogGarbage, Warning, TEXT("Failed to voice"));
		return;
	}

	/* ... TESTING! */
	UE_LOG(LogGarbage, Warning, TEXT("LOL"));

	/* This code enables the basic voice playback without spatialization
	 * StartNetworkedVoice() will enable voice playback locally
	 * RegisterLocalTalker() enables voice communication
	 */
	voice->StartNetworkedVoice(0);
	voice->RegisterLocalTalker(0);
	
	/* We only set this pointer once we know it's good */
	m_voice = voice;

	/* Voice ticking works best at 60, proved through rigorous testing /s
	 * This tick rate runs asynchronously from the primary tick rate
	 */
	float tick_rate = 1.f / 60.f;

	/* Timers allow us to run separate from the graphics tick rate, and we like this */
	GetWorld()->GetTimerManager().SetTimer(
		m_timerHandle, [&]()
	{
		/* We first check the validity ouf our voice system pointer */
		if (m_voice.IsValid())
		{
			/* ... we give it time to tick... */
			m_voice->Tick(tick_rate);

			/* and proceed to retrieve audio data.
			 * This code is not proved to work.
			 */
			for(int32_t i=0; i < m_voice->GetNumLocalTalkers(); i++)
			{
				/* Get the voice packet first */
				TSharedPtr<FVoicePacket> m_voicePacket = m_voice->GetLocalPacket(i);

				/* Check for validity of packet and sender pointer */
				if (!m_voicePacket.IsValid() || !m_voicePacket->GetSender().IsValid())
					continue;

				/* Get the sender ID */
				auto sender = m_voicePacket->GetSender()->ToString();

				/* We now proceed to create a procedural sound wave sample 
				 * We create a map from the sender ID to sound wave.
				 */
				USoundWaveProcedural* wave = nullptr;
				if (m_soundWaves.Contains(sender))
					wave = m_soundWaves[sender];
				if (!wave)
				{
					/* We must use this to construct new UObjects */
					wave = NewObject<USoundWaveProcedural>();
				}

				/* Through black magic we retrieve the voice packet's data, cast it to its implementation detail (because it's otherwise unavailable) */
				FVoicePacketImpl* packet = (FVoicePacketImpl*)(m_voicePacket.Get());
				UE_LOG(LogGarbage, Warning, TEXT("Packet sender: %s"), &sender[0]);
				UE_LOG(LogGarbage, Warning, TEXT("Packet data: %i"), packet->GetBufferSize());
				/* And we queue the data in our sound wave */
				wave->QueueAudio(packet->Buffer.GetData(), packet->GetBufferSize());
			}
		}
	}, tick_rate, true, 0.5f);

	/* This one is supposed to measure audio levels, asynchronous from the packet processing to avoid hickups */
	GetWorld()->GetTimerManager().SetTimer(m_timerFrequency, [&]()
	{
		// No implementation here!
	}, 1.f / 10.f, true, 0.5f);
}

void UVoiceSource::BeginDestroy()
{
	Super::BeginDestroy();

	/* Important to check if the world is still existing! */
	if(m_timerHandle.IsValid() && GetWorld())
		GetWorld()->GetTimerManager().ClearTimer(m_timerHandle);
	//GetWorld()->GetTimerManager().ClearTimer(m_timerFrequency);

}

// Called every frame
void UVoiceSource::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );
	// ...
}

USoundWave* UVoiceSource::GetPlayerSound(int id)
{
	/* This will in principle return a sample to be played for a given player ID
	 * The player ID will correlate to the local player IDs, enumerated from 0
	 * Player ID 0 will be the local player etc., and will typically not be played
	 */
	return nullptr;
}

float UVoiceSource::GetSoundDuration(USoundWave* wav)
{
	/* Just a helper function to retrieve the length of a USoundWave,
	 * as this cannot be done with BP.
	 */
	if(wav)
		return wav->GetDuration();
	return 0;
}