#include "gvipch.h"
#include "WavesSpawner/WaveManager.h"
#include "Core/Prefab.h"
#include "Core/EntityComponent/Entity.h"
#include "Core/EntityComponent/EntityManager.h"
#include "Ships/Player/Player.h"
#include "Ships/Enemies/Rugal.h"
#include "Ships/Enemies/BigCore.h"
#include "PowerUp/PowerUpCapsule.h"

namespace Gradius
{
    WaveManager::WaveManager(Helios::Entity& a_owner, Helios::ComponentID a_id)
        : ScriptableComponent(a_owner, a_id),
        m_playerRef(nullptr),
        m_turngearPrefab(nullptr),
        m_rugalPrefab(nullptr),
        m_krangPrefab(nullptr),
        m_bigCorePrefab(nullptr)
    {
        std::srand(static_cast<unsigned>(std::time(nullptr)));
    }

    void WaveManager::Start()
    {
        m_turngearPrefab = Helios::PrefabUtility::LoadPrefab("./Assets/Prefabs/TurnGear.prefab");
        m_krangPrefab = Helios::PrefabUtility::LoadPrefab("./Assets/Prefabs/Krang.prefab");
        m_rugalPrefab = Helios::PrefabUtility::LoadPrefab("./Assets/Prefabs/Rugal.prefab");
        m_bigCorePrefab = Helios::PrefabUtility::LoadPrefab("./Assets/Prefabs/BigCore.prefab");
        m_powerUpCapsule = Helios::PrefabUtility::LoadPrefab("./Assets/Prefabs/PowerUp Capsule.prefab");

        m_currentTime.Reset();

        // Get the player reference.
        Helios::Entity* player = Helios::EntityManager::GetInstance().GetEntityByName("Player (Clone)");
        if (player != nullptr)
        {
            m_playerRef = player->GetComponent<Player>();
        }
    }

    void WaveManager::Update(float)
    {
        CheckWaveSpawnTime();
    }

    void WaveManager::CheckWaveSpawnTime()
    {
        for (auto wave_it = m_waves.begin(); wave_it != m_waves.end();)
        {
            bool erased = false;

            if (m_currentTime.GetTimeSinceStart() >= wave_it->m_spawnTime) {
                // Spawn wave
                SpawnWave(*wave_it);

                // Remove from list.
                erased = true;
                wave_it = m_waves.erase(wave_it);
            }

            if (!erased)
            {
                ++wave_it;
            }
        }
    }

    void WaveManager::SpawnWave(const Wave& a_wave)
    {
        std::vector<Helios::Entity*> enemies;
        enemies.reserve(a_wave.m_spawnAmount);

        for (int i = 0; i < a_wave.m_spawnAmount; i++)
        {
            Helios::Entity* enemy = GetEnemyType(a_wave.m_enemyType);
            enemy->GetTransform().SetPosition(GetSpawnPosition(a_wave.m_spawnPositionType));

            enemies.push_back(enemy);
        }

        if (a_wave.m_dropsPowerUp)
        {
            const std::size_t randEnemy = std::rand() % (enemies.size() - 1);
            auto ship = enemies[randEnemy]->GetComponent<BaseShip>();
            ship->SetDropPowerUp(true);
            ship->m_onShipDestroyedCallback.append(std::bind(&WaveManager::OnShipDestroyed, this, std::placeholders::_1));
        }

        SetFormation(a_wave.m_formationType, enemies);
    }

    void WaveManager::OnShipDestroyed(BaseShip* a_ship)
    {
        Helios::Entity* powerUpEntity = m_powerUpCapsule->Spawn();
        powerUpEntity->AddComponent<PowerUpCapsule>();
        powerUpEntity->GetTransform().SetPosition(a_ship->GetTransform().GetPosition());
    }

    Helios::Entity* WaveManager::GetEnemyType(EEnemyType a_enemyType)
    {
        Helios::Entity* enemy = nullptr;

        switch (a_enemyType)
        {
        case EEnemyType::TurnGear:
        {
            enemy = m_turngearPrefab->Spawn();
        }
        break;
        case EEnemyType::Krang:
        {
            enemy = m_krangPrefab->Spawn();
        }
        break;
        case EEnemyType::Rugal:
        {
            enemy = m_rugalPrefab->Spawn();
            enemy->GetComponent<Rugal>()->m_playerRef = m_playerRef;
        }
        break;
        case EEnemyType::BigCore:
        {
            enemy = m_bigCorePrefab->Spawn();
            enemy->GetComponent<BigCore>()->m_playerRef = m_playerRef;
        }
        break;
        }

        return enemy;
    }

    glm::vec3 WaveManager::GetSpawnPosition(ESpawnPositionType a_spawnPositionType)
    {
        glm::vec3 pos = glm::vec3();

        switch (a_spawnPositionType)
        {
        case ESpawnPositionType::Top:
        {
            pos = glm::vec3(1550, 100, 0);
        }
        break;
        case ESpawnPositionType::Center:
        {
            pos = glm::vec3(1550, 400, 0);
        }
        break;
        case ESpawnPositionType::Bottom:
        {
            pos = glm::vec3(1550, 650, 0);
        }
        break;
        }

        return pos;
    }

    void WaveManager::SetFormation(EFormationType formationType, std::vector<Helios::Entity*> enemies)
    {
        const int spriteWidth = 98;
        const int spriteHeight = 98;
        const Vec3& currPos = enemies.at(0)->GetTransform().GetPosition();

        switch (formationType)
        {
        case EFormationType::Row:
        {
            for (int i = 0; i < enemies.size(); i++)
            {
                enemies[i]->GetTransform().SetPosition(currPos.x + (spriteWidth * i), currPos.y, currPos.z);
            }
        }
        break;
        case EFormationType::Column:
        {
            for (int i = 0; i < enemies.size(); i++)
            {
                enemies[i]->GetTransform().SetPosition(currPos.x, currPos.y + (spriteHeight * i), currPos.z);
            }
        }
        break;
        case EFormationType::Arrow:
        {
            int count = 1;
            for (int i = 0; i < enemies.size(); i++)
            {
                if (i != 0)
                {
                    if (i % 2 == 0)
                    {
                        enemies[i]->GetTransform().SetPosition(currPos.x + (spriteWidth * count), currPos.y + (spriteHeight * count), 0.f);
                        count++;
                    }
                    else
                    {
                        enemies[i]->GetTransform().SetPosition(currPos.x + (spriteWidth * count), currPos.y + (spriteHeight * -count), 0.f);
                    }
                }
            }
        }
        break;
        }

        formationType;
        enemies;
    }

}
