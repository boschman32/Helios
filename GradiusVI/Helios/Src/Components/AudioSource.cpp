#include "hepch.h"
#include "Components/AudioSource.h"
#include "Core/AudioEngine.h"
namespace Helios
{
	void AudioSource::Play()
	{
		if (m_channelID == -1)
		{
			m_channelID = AudioEngine::CAudioEngine::GetInstance().PlaySound(m_asset.m_filePath, AudioEngine::Vector3({ 0,0,0 }), m_decibelOffset, m_looping, m_streaming);
		}
		else if (m_streaming)
		{
			Stop();
			Play();
		}
		else
		{
			m_channelID = AudioEngine::CAudioEngine::GetInstance().PlaySound(m_asset.m_filePath, AudioEngine::Vector3({ 0,0,0 }), m_decibelOffset, m_looping, m_streaming);
		}
	}

	void AudioSource::SetFile(const std::string& a_newFile)
	{
		m_asset.m_filePath = a_newFile;
	}

	void AudioSource::Stop()
	{
		if (m_channelID != -1)
		{
			AudioEngine::CAudioEngine::GetInstance().StopSound(m_channelID);
			m_channelID = -1;
		}
	}

	bool AudioSource::IsPlaying() const
	{
		if (m_channelID != -1)
		{
			if(!AudioEngine::CAudioEngine::GetInstance().IsSoundPlaying(m_channelID))
			{
				
			}
		}
		return false;
	}

	void AudioSource::SetDecibelOffset(float a_newOffset)
	{
		if (a_newOffset >= -75 && a_newOffset <= 10.0f)
		{
			m_decibelOffset = a_newOffset;
		}
	}

	const float& AudioSource::GetDecibelOffset() const
	{
		return m_decibelOffset;
	}
}
