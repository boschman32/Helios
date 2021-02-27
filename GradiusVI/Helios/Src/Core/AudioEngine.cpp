#include "hepch.h"
#include "Core/AudioEngine.h"
namespace AudioEngine
{
	Implementation* sgpImplementation = nullptr;

	Implementation::Implementation()
	{
		mpStudioSystem = NULL;
		CAudioEngine::ErrorCheck(FMOD::Studio::System::create(&mpStudioSystem));
		CAudioEngine::ErrorCheck(mpStudioSystem->initialize(32, FMOD_STUDIO_INIT_LIVEUPDATE, FMOD_INIT_PROFILE_ENABLE, NULL));

		mpSystem = NULL;
		CAudioEngine::ErrorCheck(mpStudioSystem->getCoreSystem(&mpSystem));
	}

	Implementation::~Implementation()
	{
		CAudioEngine::ErrorCheck(mpStudioSystem->unloadAll());
		CAudioEngine::ErrorCheck(mpStudioSystem->release());
	}

	void Implementation::Update()
	{
		std::vector<ChannelMap::iterator> pStoppedChannels;
		for (auto it = mChannels.begin(), itEnd = mChannels.end(); it != itEnd; ++it)
		{
			bool bIsPlaying = false;
			it->second->isPlaying(&bIsPlaying);
			if (!bIsPlaying)
			{
				pStoppedChannels.push_back(it);
			}
		}
		for (auto& it : pStoppedChannels)
		{
			mChannels.erase(it);
		}
		CAudioEngine::ErrorCheck(mpStudioSystem->update());
	}

	void CAudioEngine::Init()
	{
		sgpImplementation = new Implementation;
	}

	void CAudioEngine::Update()
	{
		sgpImplementation->Update();
	}

	void CAudioEngine::Shutdown()
	{
		delete sgpImplementation;
	}

	int CAudioEngine::ErrorCheck(FMOD_RESULT result)
	{
		if (result != FMOD_OK) {
			HE_CORE_CRITICAL("FMOD ERROR {0}", result);
			return 1;
		}
		// cout << "FMOD all good" << endl;
		return 0;
	}

	void CAudioEngine::LoadBank(const std::string& strBankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags)
	{
		auto tFoundIt = sgpImplementation->mBanks.find(strBankName);
		if (tFoundIt != sgpImplementation->mBanks.end())
			return;
		FMOD::Studio::Bank* pBank;
		CAudioEngine::ErrorCheck(sgpImplementation->mpStudioSystem->loadBankFile(strBankName.c_str(), flags, &pBank));
		if (pBank) {
			sgpImplementation->mBanks[strBankName] = pBank;
		}
	}

	void CAudioEngine::LoadEvent(const std::string& strEventName)
	{
		auto tFoundit = sgpImplementation->mEvents.find(strEventName);
		if (tFoundit != sgpImplementation->mEvents.end())
			return;
		FMOD::Studio::EventDescription* pEventDescription = NULL;
		CAudioEngine::ErrorCheck(sgpImplementation->mpStudioSystem->getEvent(strEventName.c_str(), &pEventDescription));
		if (pEventDescription) {
			FMOD::Studio::EventInstance* pEventInstance = NULL;
			CAudioEngine::ErrorCheck(pEventDescription->createInstance(&pEventInstance));
			if (pEventInstance) {
				sgpImplementation->mEvents[strEventName] = pEventInstance;
			}
		}
	}

	void CAudioEngine::LoadSound(const std::string& strSoundName, bool b3d, bool bLooping, bool bStream)
	{
		auto tFoundIt = sgpImplementation->mSounds.find(strSoundName);
		if (tFoundIt != sgpImplementation->mSounds.end())
			return;

		FMOD_MODE eMode = FMOD_DEFAULT;
		eMode |= b3d ? FMOD_3D : FMOD_2D;
		eMode |= bLooping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
		eMode |= bStream ? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE;

		FMOD::Sound* pSound = nullptr;
		CAudioEngine::ErrorCheck(sgpImplementation->mpSystem->createSound(strSoundName.c_str(), eMode, nullptr, &pSound));
		if (pSound) {
			sgpImplementation->mSounds[strSoundName] = pSound;
		}

	}

	void CAudioEngine::UnLoadSound(const std::string& strSoundName)
	{
		auto tFoundIt = sgpImplementation->mSounds.find(strSoundName);
		if (tFoundIt == sgpImplementation->mSounds.end())
			return;

		CAudioEngine::ErrorCheck(tFoundIt->second->release());
		sgpImplementation->mSounds.erase(tFoundIt);
	}

	int CAudioEngine::PlaySound(const std::string& strSoundName, const Vector3& vPos, float fVolumedB, bool isLooped, bool isStreamed)
	{
		int nChannelId = sgpImplementation->mnNextChannelId++;
		auto tFoundIt = sgpImplementation->mSounds.find(strSoundName);
		if (tFoundIt == sgpImplementation->mSounds.end())
		{
			LoadSound(strSoundName,false,isLooped,isStreamed);
			tFoundIt = sgpImplementation->mSounds.find(strSoundName);
			if (tFoundIt == sgpImplementation->mSounds.end())
			{
				return nChannelId;
			}
		}
		FMOD_MODE eMode = FMOD_DEFAULT;
		eMode |= isLooped ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
		eMode |= isStreamed ? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE;
		tFoundIt->second->setMode(eMode);
		FMOD::Channel* pChannel = nullptr;
		CAudioEngine::ErrorCheck(sgpImplementation->mpSystem->playSound(tFoundIt->second, nullptr, true, &pChannel));
		if (pChannel)
		{
			FMOD_MODE currMode;
			tFoundIt->second->getMode(&currMode);
			if (currMode & FMOD_3D) {
				FMOD_VECTOR position = VectorToFmod(vPos);
				CAudioEngine::ErrorCheck(pChannel->set3DAttributes(&position, nullptr));
			}
			CAudioEngine::ErrorCheck(pChannel->setVolume(dbToVolume(fVolumedB)));
			CAudioEngine::ErrorCheck(pChannel->setPaused(false));
			sgpImplementation->mChannels[nChannelId] = pChannel;
		}
		return nChannelId;
	}

	int CAudioEngine::PlaySoundStreamed(const std::string& strSoundName, const Vector3& vPos, float fVolumedB,
		bool looped)
	{
		return PlaySound(strSoundName, vPos, fVolumedB, looped, true);
	}

	void CAudioEngine::StopSound(int channelID)
	{
		if (sgpImplementation->mChannels.size() - 1 <= channelID)
		{
			sgpImplementation->mChannels[channelID]->stop();
		}
	}

	bool CAudioEngine::IsSoundPlaying(int channelID)
	{
		bool playing = false;
		if (sgpImplementation->mChannels.size() - 1 <= channelID)
		{
			sgpImplementation->mChannels[channelID]->isPlaying(&playing);
		}
		return playing;
	}

	void CAudioEngine::PlayEvent(const std::string& strEventName)
	{
		auto tFoundit = sgpImplementation->mEvents.find(strEventName);
		if (tFoundit == sgpImplementation->mEvents.end()) {
			LoadEvent(strEventName);
			tFoundit = sgpImplementation->mEvents.find(strEventName);
			if (tFoundit == sgpImplementation->mEvents.end())
				return;
		}
		tFoundit->second->start();
	}

	void CAudioEngine::StopEvent(const std::string& strEventName, bool bImmediate)
	{
		auto tFoundIt = sgpImplementation->mEvents.find(strEventName);
		if (tFoundIt == sgpImplementation->mEvents.end())
			return;

		FMOD_STUDIO_STOP_MODE eMode;
		eMode = bImmediate ? FMOD_STUDIO_STOP_IMMEDIATE : FMOD_STUDIO_STOP_ALLOWFADEOUT;
		CAudioEngine::ErrorCheck(tFoundIt->second->stop(eMode));
	}

	void CAudioEngine::GeteventParameter(const std::string& strEventName, const std::string& strEventParameter,
		float* parameter)
	{
		auto tFoundIt = sgpImplementation->mEvents.find(strEventName);
		if (tFoundIt == sgpImplementation->mEvents.end())
			return;

		tFoundIt->second->getParameterByName(strEventParameter.c_str(), parameter);
	}

	void CAudioEngine::SetEventParameter(const std::string& strEventName, const std::string& strParameterName,
		float fValue)
	{
		auto tFoundIt = sgpImplementation->mEvents.find(strEventName);
		if (tFoundIt == sgpImplementation->mEvents.end())
			return;

		tFoundIt->second->setParameterByName(strParameterName.c_str(), fValue);
	}

	void CAudioEngine::StopAllChannels()
	{
		for(auto a : sgpImplementation->mChannels)
		{
			a.second->stop();
		}
	}

	void CAudioEngine::SetPausedForAllChannels(bool isPaused)
	{
		for (auto a : sgpImplementation->mChannels)
		{
			a.second->setPaused(isPaused);
		}
	}

	void CAudioEngine::SetChannel3dPosition(int nChannelId, const Vector3& vPosition)
	{
		auto tFoundIt = sgpImplementation->mChannels.find(nChannelId);
		if (tFoundIt == sgpImplementation->mChannels.end())
			return;

		FMOD_VECTOR position = VectorToFmod(vPosition);
		CAudioEngine::ErrorCheck(tFoundIt->second->set3DAttributes(&position, NULL));
	}

	void CAudioEngine::SetChannelvolume(int nChannelId, float fVolumedB)
	{
		auto tFoundIt = sgpImplementation->mChannels.find(nChannelId);
		if (tFoundIt == sgpImplementation->mChannels.end())
			return;

		CAudioEngine::ErrorCheck(tFoundIt->second->setVolume(dbToVolume(fVolumedB)));
	}

	bool CAudioEngine::IsEventPlaying(const std::string& strEventName) const
	{
		auto tFoundIt = sgpImplementation->mEvents.find(strEventName);
		if (tFoundIt == sgpImplementation->mEvents.end())
			return false;

		FMOD_STUDIO_PLAYBACK_STATE* state = NULL;
		if (tFoundIt->second->getPlaybackState(state) == FMOD_STUDIO_PLAYBACK_PLAYING) {
			return true;
		}
		return false;
	}

	float CAudioEngine::dbToVolume(float db)
	{
		return std::powf(10.0f, 0.05f * db);
	}

	float CAudioEngine::VolumeTodb(float volume)
	{
		return 20.0f * std::log10f(volume);
	}

	FMOD_VECTOR CAudioEngine::VectorToFmod(const Vector3& vPosition)
	{
		FMOD_VECTOR fVec;
		fVec.x = vPosition.x;
		fVec.y = vPosition.y;
		fVec.z = vPosition.z;
		return fVec;
	}
}
