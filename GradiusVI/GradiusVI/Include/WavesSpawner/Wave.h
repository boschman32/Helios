#pragma once

namespace Gradius
{
    enum class EEnemyType
    {
        TurnGear,
        Krang,
        Rugal,
        BigCore
    };

    enum class ESpawnPositionType
    {
        Top,
        Center,
        Bottom
    };

    enum class EFormationType
    {
        Row,
        Column,
        Arrow
    };

    class Wave
    {
        RTTR_ENABLE()
    public:
        float m_spawnTime = 0;
        int m_spawnAmount = 0;
        bool m_dropsPowerUp = false;
        EEnemyType m_enemyType = EEnemyType::TurnGear;
        ESpawnPositionType m_spawnPositionType = ESpawnPositionType::Center;
        EFormationType m_formationType = EFormationType::Row;
        RTTR_REGISTRATION_FRIEND;
    };
}

