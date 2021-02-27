#include "hepch.h"
#include "Core/Prefab.h"
#include "Core/ResourceManager.h"
#include "Core/Core.h"
#include "Core/EntityComponent/EntityManager.h"

#include "SceneManagement/SceneManager.h"

#include "Components/Transform.h"

#include "Utils/Serializer.h"
#include "Utils/FileUtils.h"
#include "Core/Application.h"

#include "Editor/ContentBrowser.h"

namespace Helios
{
	std::unordered_map<std::string, std::pair<std::uint32_t, std::shared_ptr<Prefab>>> PrefabUtility::m_prefabUseCount;

	Prefab::Prefab()
		: Resource(ResourceType::PREFAB)
	{

	}

	void Prefab::OnInitialize()
	{
		if (!ReadFromFileToString(m_path, m_prefabData))
		{
			HE_CORE_CRITICAL("Couldn't load prefab data: {0}", m_path);
		}
	}

	void Prefab::Reload()
	{
		if (!ReadFromFileToString(m_path, m_prefabData))
		{
			HE_CORE_CRITICAL("Couldn't load prefab data: {0}", m_path);
		}
	}

	Entity* Prefab::Spawn(const Vec3& a_spawnPos, bool a_useSerializedPos) const
	{
		rapidjson::Document doc;
		if (doc.Parse(m_prefabData.c_str()).HasParseError())
		{
			HE_CORE_ERROR("Cannot deserialize Prefab");
			HE_CORE_ASSERT(false, "Can't deserialize prefab because JSON parse error!");
			return nullptr;
		}

		Entity* prefabEntity = Deserializer::DeserializePrefab(doc, m_path, true, false, Application::Get().IsInEditMode());

		if (!a_useSerializedPos)
		{
			prefabEntity->GetTransform().SetPosition(a_spawnPos);
		}

		return prefabEntity;
	}

	Prefab* PrefabUtility::LoadPrefab(const std::string& a_path)
	{
		const std::string truePath = file_data::ROOT_FOLDER + std::filesystem::relative(a_path).string();
		std::shared_ptr<Prefab> prefab = ResourceManager::GetInstance().LoadResource<Prefab>(truePath);

		const auto found = m_prefabUseCount.find(truePath);
		if (found != m_prefabUseCount.end())
		{
			found->second.first++;
		}
		else
		{
			m_prefabUseCount[truePath] = std::make_pair(1, prefab);
		}
		return prefab.get();
	}

	void PrefabUtility::UpdatePrefabResource(const std::string& a_prefabPath)
	{
		if (m_prefabUseCount.find(a_prefabPath) != m_prefabUseCount.end())
		{
			const std::shared_ptr<Prefab>& prefab = m_prefabUseCount.at(a_prefabPath).second;
			ResourceManager::GetInstance().GetResource<Prefab>(prefab->GetID())->Reload();
		}
	}

	void PrefabUtility::UpdateClones(const EntityID& updateCloneId, const std::string& a_prefabPath, const std::string& a_prefabMeta)
	{
		rapidjson::Document doc;

		if (doc.Parse(a_prefabMeta.c_str()).HasParseError())
		{
			HE_CORE_ERROR("Cannot deserialize Prefab");
			HE_CORE_ASSERT(false, "Can't deserialize prefab because JSON parse error!");
			return;
		}

		Scene& activeScene = SceneManager::GetInstance().GetActiveScene();
		for (auto e : EntityManager::GetInstance().GetAllEntities())
		{
			//Find an entity that also uses this prefab and no need to update ourselves.
			if (e->GetID() != updateCloneId && e->GetPrefabPath() == a_prefabPath)
			{
				Transform* parent = e->GetTransform().GetParent();
				Transform originalTransform = e->GetTransform();

				EntityManager::GetInstance().DestroyEntityDirectly(e, activeScene);
				Entity* updatedEntity = Deserializer::DeserializePrefab(doc, a_prefabPath);
				updatedEntity->GetTransform().SetLocalPosition(originalTransform.GetLocalPosition());
				updatedEntity->GetTransform().SetLocalRotation(originalTransform.GetLocalRotation());
				updatedEntity->GetTransform().SetScale(originalTransform.GetScale());
				
				//If we had a parent reattach ourselves to it.
				if (parent != nullptr)
				{
					updatedEntity->GetTransform().SetParent(parent);
				}
			}
		}
	}

	void PrefabUtility::UnloadPrefab(const std::string& a_path)
	{
		std::pair<std::uint32_t, std::shared_ptr<Prefab>>& prefabUses = m_prefabUseCount.at(a_path);
		prefabUses.first--;

		if (prefabUses.first <= 0)
		{
			ResourceManager::GetInstance().UnloadResource(std::move(prefabUses.second));
			m_prefabUseCount.erase(a_path);
		}
	}

	void PrefabUtility::UnloadAll()
	{
		for (auto& p : m_prefabUseCount)
		{
			ResourceManager::GetInstance().UnloadResource(std::move(p.second.second));
		}
		m_prefabUseCount.clear();
	}

}