#include "gvipch.h"
#include "Bullets/BaseBullet.h"


namespace Gradius
{
    BaseBullet::BaseBullet(Helios::Entity& a_owner, const Helios::ComponentID& a_id)
        : ScriptableComponent(a_owner, a_id)
    {
       
    }

    BaseBullet::~BaseBullet()
    {
        if (m_onDestroyCallback != nullptr)
        {
            m_onDestroyCallback(*this);
        }
    }
}
