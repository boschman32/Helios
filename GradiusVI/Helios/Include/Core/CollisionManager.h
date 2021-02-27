#pragma once
#include "Components/Colliders/Collider.h"
namespace Helios
{
	class CollisionManager
	{
	public:
		static CollisionManager& GetInstance()
		{
			static CollisionManager instance;

			return instance;
		}

		enum ECollisionType
		{
			ECollisionType_Begin, ECollisionType_Overlap, ECollisionType_End
		};

		void NotifyScriptableComponentOfOverlap(ECollisionType a_type, Entity* a_entity, Collider* a_collider) const;

        void Render();

		void AddCollider(Collider* a_collider);
		void RemoveCollider(Collider* a_collider);
		void CollisionChecks();
		std::vector<Collider*> const& GetColliders() const;
		void Reset();
	private:
		std::vector<Collider*> m_allColliders;
		std::vector<Collider*> m_collidersOnScreen;

		bool IsInScreen(const Collider& a_c);
	};
}