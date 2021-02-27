#pragma once
#include "IPowerUp.h"
#include "Components/AudioSource.h"
#include "Components/UIComponent.h"

namespace Gradius
{
    class Player;
    
    class BasePowerUp: public IPowerUp
    {
    public:
        BasePowerUp(std::string a_selectedSprite, std::string a_unselectedSprite, std::string a_audioPath) :
            m_selectedSprite(std::move(a_selectedSprite)),
            m_unselectedSprite(std::move(a_unselectedSprite)),
            m_audioPath(std::move(a_audioPath)) {};
        virtual ~BasePowerUp() { };

        void Activate() override {};

        void Selected() const
        {
          m_image->SetTextureFile(R"(.//Assets//Sprites//Power_Up//)" + m_selectedSprite + ".png");
        }

        void Unselected() const
        {
           m_image->SetTextureFile(R"(.//Assets//Sprites//Power_Up//)" + m_unselectedSprite + ".png");
        }

        virtual void DisablePowerUp()
        {
            m_selectable = false;
            m_selectedSprite = "Empty_Selected";
            m_unselectedSprite = "Empty_Unselected";
        }

        virtual void ResetPowerUp()
        {
            m_selectable = true;
            Unselected();
        }

        Helios::UIComponent* m_image = nullptr;
        Helios::AudioSource* m_audioSource = nullptr;
        Player* m_playerRef = nullptr;
        bool m_selectable = true;

    protected:
        std::string m_selectedSprite;
        std::string m_unselectedSprite;
        std::string m_audioPath;
    };
}
