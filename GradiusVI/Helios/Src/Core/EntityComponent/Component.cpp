#include "hepch.h"
#include "Core/EntityComponent/Entity.h"
#include "Core/EntityComponent/Component.h"

namespace Helios
{
	Component::Component(Entity& a_owner, const ComponentID& a_id)
	: m_owner(&a_owner), m_ownerId(&a_owner.GetID()), m_isEnabled(true), m_id(a_id)
	{
		
	}

	Transform& Component::GetTransform() const
	{
		HE_CORE_ASSERT(m_owner != nullptr, "Component has no owner so we can't get the transform from it!");
		return m_owner->GetTransform();
	}
}



