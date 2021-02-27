#include "hepch.h"
#include "Components/RenderComponent.h"
#include "Components/Transform.h"
#include "Core/EntityComponent/Entity.h"

namespace Helios
{
	RenderComponent::RenderComponent(Entity& a_owner, const ComponentID& a_id)
		: Component(a_owner, a_id)
	{
		
	}
}

