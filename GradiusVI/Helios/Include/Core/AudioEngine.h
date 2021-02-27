#pragma once

namespace AudioEngine
{
	struct Vector3 {
		float x;
		float y;
		float z;
	};

	struct Implementation
	{
		Implementation();
		~Implementation();

		void Update();

		FMOD::Studio::System* mpStudioSystem;
		FMOD::System* mpSystem;

		int mnNextChannelId;

		using SoundMap = std::map<std::string, FMOD::Sound*>;
		using ChannelMap = std::map<int, FMOD::Channel*>;
		using EventMap = std::map<std::string, FMOD::Studio::EventInstance*>;
		using BankMap = std::map<std::string, FMOD::Studio::Bank*>;

		BankMap mBanks;
		EventMap mEvents;
		SoundMap mSounds;
		ChannelMap mChannels;
	};

	class CAudioEngine {
	public:
		CAudioEngine() = default;
		static CAudioEngine& GetInstance()
		{
			static CAudioEngine instance;

			return instance;
		}
		CAudioEngine(const CAudioEngine&) = delete;
		void operator=(const CAudioEngine&) = delete;
		static void Init();
		static void Update();
		static void Shutdown();
		static int ErrorCheck(FMOD_RESULT result);

		void LoadBank(const std::string& strBankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags);
		void LoadEvent(const std::string& strEventName);
		void LoadSound(const std::string& strSoundName, bool b3d = true, bool bLooping = false, bool bStream = false);
		void UnLoadSound(const std::string& strSoundName);
		int PlaySound(const std::string& strSoundName, const Vector3& vPos = Vector3{ 0, 0, 0 }, float fVolumedB = 0.0f, bool looped = false, bool isStreamed = false);
		int PlaySoundStreamed(const std::string& strSoundName, const Vector3& vPos = Vector3{ 0, 0, 0 }, float fVolumedB = 0.0f, bool looped = false);
		void StopSound(int channelID);
		bool IsSoundPlaying(int channelID);
		void PlayEvent(const std::string& strEventName);
		void StopEvent(const std::string& strEventName, bool bImmediate = false);\
		
		void GeteventParameter(const std::string& strEventName, const std::string& strEventParameter, float* parameter);
		void SetEventParameter(const std::string& strEventName, const std::string& strParameterName, float fValue);
		void StopAllChannels();
		void SetPausedForAllChannels(bool isPaused);
		void SetChannel3dPosition(int nChannelId, const Vector3& vPosition);
		void SetChannelvolume(int nChannelId, float fVolumedB);
		bool IsEventPlaying(const std::string& strEventName) const;
		float dbToVolume(float db);
		float VolumeTodb(float volume);
		FMOD_VECTOR VectorToFmod(const Vector3& vPosition);
	};
}