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

	auto online = IOnlineSubsystem::Get("Steam");
	if (!online)
	{
		UE_LOG(LogGarbage, Warning, TEXT("Failed to online"));
		return;
	}

	auto voice = online->GetVoiceInterface();
	if (!voice.IsValid())
	{
		UE_LOG(LogGarbage, Warning, TEXT("Failed to voice"));
		return;
	}

	UE_LOG(LogGarbage, Warning, TEXT("LOL"));

	voice->StartNetworkedVoice(0);
	voice->RegisterLocalTalker(0);
	
	m_voice = voice;

	float tick_rate = 1.f / 100.f;

	GetWorld()->GetTimerManager().SetTimer(
		m_timerHandle, [&]()
	{
		if (m_voice.IsValid())
		{
			m_voice->Tick(tick_rate);

			for(int32_t i=0; i < m_voice->GetNumLocalTalkers(); i++)
			{
				m_voicePacket = m_voice->GetLocalPacket(i);

				if (!m_voicePacket.IsValid() || !m_voicePacket->GetSender().IsValid())
					continue;

				auto sender = m_voicePacket->GetSender()->ToString();

				USoundWaveProcedural* wave = nullptr;
				if (m_soundWaves.Contains(sender))
					wave = m_soundWaves[sender];
				if (!wave)
				{
					wave = NewObject<USoundWaveProcedural>();
				}

				FVoicePacketImpl* packet = (FVoicePacketImpl*)(m_voicePacket.Get());
				UE_LOG(LogGarbage, Warning, TEXT("Packet data: %i"), packet->GetBufferSize());
				wave->QueueAudio(packet->Buffer.GetData(), packet->GetBufferSize());

				m_voicePacket.Reset();
			}
		}
	}, tick_rate, true, 0.5f);

	GetWorld()->GetTimerManager().SetTimer(m_timerFrequency, [&]()
	{
		
	}, 1.f / 10.f, true, 0.5f);
}

void UVoiceSource::BeginDestroy()
{
	Super::BeginDestroy();

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

