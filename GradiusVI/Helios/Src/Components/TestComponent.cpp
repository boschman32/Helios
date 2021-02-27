#include "hepch.h"
#include "Components/TestComponent.h"
#include "Core/EntityComponent/Entity.h"
#include "COre/InputManager.h"
#include "Components/AudioSource.h"

namespace Helios
{
	TestComponent::TestComponent(Entity& a_owner, const ComponentID& a_id) : ScriptableComponent(a_owner, a_id)
	{
        
	}

    void TestComponent::Start()
    {
       
    }

	void TestComponent::Update(float)
	{
		if(InputManager::GetController(1)->GetButtonOnce("Fire"))
		{
			m_owner->AddComponent<AudioSource>();
			m_owner->GetComponent<AudioSource>()->SetFile("./Assets/Sounds/Play/STAR WARS/LaserXWING.WAV");
			m_owner->GetComponent<AudioSource>()->m_looping = true;
			m_owner->GetComponent<AudioSource>()->Play();
		}
	}
}
