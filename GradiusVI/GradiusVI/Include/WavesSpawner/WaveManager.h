#pragma once
#include "Components/ScriptableComponent.h"
#include "WavesSpawner/Wave.h"
#include "Utils/Timer.h"

namespace Helios
{
    class Prefab;
    class Entity;
}

namespace Gradius
{
    class BaseShip;
    class Player;
    class WaveManager : public Helios::ScriptableComponent
    {
        RTTR_ENABLE(Component, Helios::ScriptableComponent)
    public:
        WaveManager(Helios::Entity& a_owner, Helios::ComponentID a_id);

        void Start() override;
        void Update(float) override;

    private:
        void CheckWaveSpawnTime();
        void SpawnWave(const Wave& a_wave);
        Helios::Entity* GetEnemyType(EEnemyType a_enemyType);
        glm::vec3 GetSpawnPosition(ESpawnPositionType a_spawnPositionType);
        void SetFormation(EFormationType, std::vector<Helios::Entity*>);

        std::vector<Wave> m_waves;
        Helios::Timer<> m_currentTime;

        Player* m_playerRef;

        // Prefabs
        Helios::Prefab* m_turngearPrefab;
        Helios::Prefab* m_rugalPrefab;
        Helios::Prefab* m_krangPrefab;
        Helios::Prefab* m_bigCorePrefab;
        Helios::Prefab* m_powerUpCapsule;
        void OnShipDestroyed(BaseShip* a_ship);
        RTTR_REGISTRATION_FRIEND;
    };
}
