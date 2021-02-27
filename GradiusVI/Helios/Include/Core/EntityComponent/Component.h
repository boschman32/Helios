#pragma once
#include "Core/Core.h"
#include <rttr/registration.h>
#include <rttr/registration_friend.h>
namespace Helios
{
	class Transform;
	
	class Component
	{
		RTTR_ENABLE()
	public:
		Component(Entity& a_owner, const ComponentID& a_id);

		Component()
			: m_owner(nullptr), m_ownerId(nullptr), m_isEnabled(true), m_id({}) {};
		virtual ~Component() = default;

        virtual void OnCreate() { };
		Entity& GetOwner() const { return *m_owner; }
 		bool IsEnabled() const { return m_isEnabled; }
		void SetEnabled(bool a_enabled) { m_isEnabled = a_enabled; }

		ComponentID GetID() const { return m_id; };

		Transform& GetTransform() const;
	protected:
		Entity* m_owner;
		const EntityID* m_ownerId;
		bool m_isEnabled;
		ComponentID m_id;

        RTTR_REGISTRATION_FRIEND
	};
}
