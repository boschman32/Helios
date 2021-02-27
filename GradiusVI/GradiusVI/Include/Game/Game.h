#pragma once
#include "Core/Application.h"

namespace Gradius
{
    class Game : public Helios::Application
    {
    public:
        Game();
    };

    class UILayer : public Helios::Layer
    {
    public:
        UILayer();

        void OnAttach() override;
        void OnPlay() override;
        void OnDetach() override;
    private:
        Helios::Entity* m_playerLivesUI;
    };
}
