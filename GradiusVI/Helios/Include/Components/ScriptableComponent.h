#pragma once
#include "Core/EntityComponent/Component.h"

namespace Helios
{
	class Collider;
	class ScriptableComponent : public Component
	{
		RTTR_ENABLE(Component)
		RTTR_REGISTRATION_FRIEND
	public:
		ScriptableComponent(Entity& a_owner, ComponentID a_id) : Component(a_owner, a_id) {};
        virtual ~ScriptableComponent() = default;

        
		virtual void Start(){};
		virtual void Update(float){};

		virtual void OnOverlapBegin(Collider* ){};
		virtual void OnOverlap(Collider* ){};
		virtual void OnOverlapEnd(Collider* ){};
	};
}
