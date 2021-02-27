#include "hepch.h"
#include "Core/CollisionManager.h"
#include "Core/EntityComponent/Entity.h"
namespace Helios
{
	void CollisionManager::NotifyScriptableComponentOfOverlap(ECollisionType a_type, Entity* a_entity, Collider* a_collider) const
	{
		OPTICK_EVENT();
		if (a_entity->GetScriptableComponents().empty())
		{
			return;
		}
		switch (a_type)
		{
		case ECollisionType_Begin:
			for (auto s : a_entity->GetScriptableComponents())
			{
				s->OnOverlapBegin(a_collider);
			}
			break;
		case ECollisionType_Overlap:
			for (auto s : a_entity->GetScriptableComponents())
			{
				s->OnOverlap(a_collider);
			}
			break;
		case ECollisionType_End:
			for (auto s : a_entity->GetScriptableComponents())
			{
				s->OnOverlapEnd(a_collider);
			}
			break;
		default:;
		}
	}

	void CollisionManager::AddCollider(Collider* a_collider)
	{
        //HE_CORE_CRITICAL("Add collider!");
		m_allColliders.push_back(a_collider);
	}

	void CollisionManager::RemoveCollider(Collider* a_collider)
	{
        //HE_CORE_CRITICAL("Remove collider!");
		m_allColliders.erase(std::remove(m_allColliders.begin(), m_allColliders.end(), a_collider), m_allColliders.end());
	}

	void CollisionManager::CollisionChecks()
	{
		OPTICK_CATEGORY(OPTICK_FUNC, Optick::Category::Physics);
		OPTICK_EVENT();
		int called = 0;
		if (m_allColliders.empty())
		{
			return;
		}

		for (auto a : m_allColliders)
		{
			if (a->IsEnabled() && a->GetOwner().IsEnabled())
			{
				if (!IsInScreen(*a))
				{
					if (a->GetIsColliding())
					{
						NotifyScriptableComponentOfOverlap(ECollisionType_End, &a->GetOwner(), nullptr);
						a->SetIsColliding(false);
					}
					continue;
				}
				for (auto b : m_allColliders)
				{
					if (b->IsEnabled() && b->GetOwner().IsEnabled())
					{
						if (a != b && &a->GetOwner() != &b->GetOwner())
						{
							called++;

							if (a->OverlapsWith(*b))
							{
								//Collision!
								if (!a->GetIsColliding())
								{
									a->SetIsColliding(true);
									NotifyScriptableComponentOfOverlap(ECollisionType_Begin, &a->GetOwner(), b);
								}

								NotifyScriptableComponentOfOverlap(ECollisionType_Overlap, &a->GetOwner(), b);
							}
							else
							{
								if (a->GetIsColliding())
								{
									NotifyScriptableComponentOfOverlap(ECollisionType_End, &a->GetOwner(), b);
									a->SetIsColliding(false);
								}
							}
						}
					}
				}
			}
		}
		m_collidersOnScreen.clear();
	}

    void CollisionManager::Render()
    {
		OPTICK_EVENT();

		for (auto c : m_allColliders)
		{
			if (c->IsEnabled() && c->GetOwner().IsEnabled())
			{
				c->Render();
			}
		}
	}

	std::vector<Collider*> const& CollisionManager::GetColliders() const
	{
		return m_allColliders;
	}

	void CollisionManager::Reset()
	{
		m_allColliders.clear();
	}

	bool CollisionManager::IsInScreen(const Collider& a_c)
	{
		return a_c.GetOwner().GetTransform().GetPosition().x > -450 &&
			a_c.GetOwner().GetTransform().GetPosition().y > -100 &&
			a_c.GetOwner().GetTransform().GetPosition().x < 1500 &&
			a_c.GetOwner().GetTransform().GetPosition().y < 900;
	}

}
