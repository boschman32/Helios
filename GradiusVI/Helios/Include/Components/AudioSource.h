#pragma once
#include "Core/EntityComponent/Component.h"
#include "Core/AudioAsset.h"
namespace Helios
{
	class AudioSource : public Component
	{
		RTTR_ENABLE(Component)
		RTTR_REGISTRATION_FRIEND
	public:
		AudioSource(Entity& a_owner, const ComponentID& a_id) : Component(a_owner, a_id) {};
		bool m_looping = false;
		bool m_streaming = false;

		AudioAsset m_asset;
		
		void Play();
		void SetFile(const std::string & a_newFile);
		void Stop();
		void SetDecibelOffset(float a_newOffset = 0.0f);
		const float& GetDecibelOffset() const ;
		bool IsPlaying() const;
	private:
		
		int m_channelID = -1;
		float m_decibelOffset = 0.0f;

	};
}