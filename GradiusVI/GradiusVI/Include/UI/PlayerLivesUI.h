#pragma once
#include "Components/ScriptableComponent.h"

namespace Helios
{
    class Entity;
    class AudioSource;
}

namespace Gradius
{
    class Player;
    class PlayerLivesUI : public Helios::ScriptableComponent
    {
        RTTR_ENABLE(Component,Helios::ScriptableComponent)
    public:
        PlayerLivesUI(Helios::Entity& owner, Helios::ComponentID id)
            : ScriptableComponent(owner, id) {};

        void UpdateLives(const int a_playerLives);
    private:
        void Start() override;

        std::vector<Helios::Entity*> m_livesSprites;
        Helios::AudioSource* m_audioSource = nullptr;
        RTTR_REGISTRATION_FRIEND;
    };
}
