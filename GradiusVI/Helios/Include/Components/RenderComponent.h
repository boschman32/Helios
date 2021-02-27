#pragma once
#include "Core/EntityComponent/Component.h"
#include "Renderer/Mesh.h"

struct UIQuadData;
namespace Helios
{
	class Transform;
	class RenderComponent : public Component
	{
	RTTR_ENABLE(Component)
	public:
		RenderComponent(Entity& a_owner, const ComponentID& a_id);
		virtual ~RenderComponent(){};
		
		virtual void Render() = 0;

		virtual const Mesh* GetRenderable() { return nullptr; };
		virtual const UIQuadData* Get2DRenderable() { return nullptr; }
	};
}