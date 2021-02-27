#include "gvipch.h"
#include "KillBox/KillBox.h"
#include "Ships/BaseShip/BaseShip.h"
#include "Collider/ColliderTags.h"
#include "Core/EntityComponent/EntityManager.h"

namespace Gradius
{
    void KillBox::OnOverlapBegin(Helios::Collider* a_col)
    {
        if (a_col->GetName() == Tags::m_enemyTag)
        {
            Helios::EntityManager::GetInstance().DestroyEntity(&a_col->GetOwner());
        }
    }
}
