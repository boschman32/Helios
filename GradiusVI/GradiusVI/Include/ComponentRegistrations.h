#pragma once
#include <rttr/registration>
#include "Ships/Player/Player.h"

#include "Ships/BaseShip/BaseShip.h"
#include "Ships/Enemies/TurnGear.h"
#include "Ships/Enemies/Rugal.h"
#include "Ships/Enemies/Krang.h"
#include "Ships/Enemies/BigCore.h"
#include "Ships/Enemies/BigCoreGate.h"
#include "Ships/Enemies/BigCoreCore.h"

#include "WavesSpawner/WaveManager.h"
#include "WavesSpawner/Wave.h"

#include "KillBox/KillBox.h"

#include "Weapons/WeaponComponent.h"

#include "Bullets/DefaultBullet.h"
#include "Bullets/MissileBullet.h"
#include "Bullets/DoubleBullet.h"
#include "Bullets/LaserBullet.h"
#include "Bullets/BigCoreBullet.h"

#include "PowerUp/PowerUpCapsule.h"
#include "PowerUp/PowerUpMeter.h"

#include "UI/PlayerLivesUI.h"
#include "Ships/Player/PlayerShieldComponent.h"

RTTR_REGISTRATION
{
    // Base ship
    RTTR_REGISTER_COMPONENT(Gradius::BaseShip, "BaseShip", Helios::Entity&, Helios::ComponentID)
        RTTR_PROPERTY("Health", Gradius::BaseShip::m_shipHealth)
        RTTR_PROPERTY("Movement Speed", Gradius::BaseShip::m_shipSpeed);

    // Player.
    RTTR_REGISTER_COMPONENT(Gradius::Player, "Player", Helios::Entity&, Helios::ComponentID)
    RTTR_PROPERTY("Banking angle", Gradius::Player::m_bankingAngle)
    RTTR_PROPERTY("Banking speed", Gradius::Player::m_bankingSpeed);

    // Enemies.
    RTTR_REGISTER_COMPONENT(Gradius::TurnGear, "TurnGear", Helios::Entity&, Helios::ComponentID)
        RTTR_PROPERTY("Rotation Speed", Gradius::TurnGear::m_rotationSpeed);

    RTTR_REGISTER_COMPONENT(Gradius::Rugal, "Rugal", Helios::Entity&, Helios::ComponentID);

    RTTR_REGISTER_COMPONENT(Gradius::Krang, "Krang", Helios::Entity&, Helios::ComponentID)
        RTTR_PROPERTY("Frequency", Gradius::Krang::m_frequency)
        RTTR_PROPERTY("Amplitude", Gradius::Krang::m_amplitude);

    RTTR_REGISTER_COMPONENT(Gradius::BigCore, "BigCore", Helios::Entity&, Helios::ComponentID);
    RTTR_REGISTER_COMPONENT(Gradius::BigCoreGate, "BigCoreGate", Helios::Entity&, Helios::ComponentID);
    RTTR_REGISTER_COMPONENT(Gradius::BigCoreCore, "BigCoreCore", Helios::Entity&, Helios::ComponentID);

    // Wave Spawner.
    RTTR_REGISTER_COMPONENT(Gradius::WaveManager, "WaveManager", Helios::Entity&, Helios::ComponentID)
        RTTR_PROPERTY("Waves", Gradius::WaveManager::m_waves);

    RTTR_REGISTER_CLASS(Gradius::Wave, "Wave")
        RTTR_PROPERTY("Spawn Time", Gradius::Wave::m_spawnTime)
        RTTR_PROPERTY("Enemy Type", Gradius::Wave::m_enemyType)
        RTTR_PROPERTY("Spawn Amount", Gradius::Wave::m_spawnAmount)
        RTTR_PROPERTY("Can drop power up", Gradius::Wave::m_dropsPowerUp)
        RTTR_PROPERTY("Spawn Position Type", Gradius::Wave::m_spawnPositionType)
        RTTR_PROPERTY("Formation Type", Gradius::Wave::m_formationType);

    RTTR_REGISTER_ENUM(Gradius::EEnemyType, "EEnemyType",
        rttr::value<Gradius::EEnemyType>("TurnGear", Gradius::EEnemyType::TurnGear),
        rttr::value<Gradius::EEnemyType>("Krang", Gradius::EEnemyType::Krang),
        rttr::value<Gradius::EEnemyType>("Rugal", Gradius::EEnemyType::Rugal),
        rttr::value<Gradius::EEnemyType>("BigCore", Gradius::EEnemyType::BigCore));

    RTTR_REGISTER_ENUM(Gradius::ESpawnPositionType, "EEnemyType",
        rttr::value<Gradius::ESpawnPositionType>("Top", Gradius::ESpawnPositionType::Top),
        rttr::value<Gradius::ESpawnPositionType>("Center", Gradius::ESpawnPositionType::Center),
        rttr::value<Gradius::ESpawnPositionType>("Bottom", Gradius::ESpawnPositionType::Bottom));

    RTTR_REGISTER_ENUM(Gradius::EFormationType, "EFormationType",
        rttr::value<Gradius::EFormationType>("Row", Gradius::EFormationType::Row),
        rttr::value<Gradius::EFormationType>("Column", Gradius::EFormationType::Column),
        rttr::value<Gradius::EFormationType>("Arrow", Gradius::EFormationType::Arrow));

    // Power-Up.
    RTTR_REGISTER_COMPONENT(Gradius::PowerUpCapsule, "PowerUp Capsule", Helios::Entity&, Helios::ComponentID);
    RTTR_REGISTER_COMPONENT(Gradius::PowerUpMeter, "PowerUp Meter", Helios::Entity&, Helios::ComponentID);

    //Base weapon
    RTTR_REGISTER_CLASS(Gradius::WeaponData, "WeaponData")
        RTTR_PROPERTY("Weapon Type", Gradius::WeaponData::m_weaponType)
        RTTR_PROPERTY("Reload time", Gradius::WeaponData::m_reloadTime)
        RTTR_PROPERTY("Max bullets", Gradius::WeaponData::m_maxBullets)
        RTTR_PROPERTY("Bullet prefab", Gradius::WeaponData::m_bulletPrefab)
        RTTR_PROPERTY("Shoot sound", Gradius::WeaponData::m_shootSound);

    // Weapon component
    RTTR_REGISTER_COMPONENT(Gradius::WeaponComponent, "Weapon Component", Helios::Entity&, const Helios::ComponentID&)
        RTTR_PROPERTY("WeaponData", Gradius::WeaponComponent::m_weaponData)
        RTTR_PROPERTY("Nozzle", Gradius::WeaponComponent::m_nozzle);

    RTTR_REGISTER_ENUM(Gradius::EWeaponType, "EWeaponType", 
            rttr::value<Gradius::EWeaponType>("Default", Gradius::EWeaponType::DefaultWeapon),
            rttr::value<Gradius::EWeaponType>("Missile", Gradius::EWeaponType::Missile),
            rttr::value<Gradius::EWeaponType>("Double", Gradius::EWeaponType::Double),
            rttr::value<Gradius::EWeaponType>("Laser", Gradius::EWeaponType::Laser),
            rttr::value<Gradius::EWeaponType>("BigCore", Gradius::EWeaponType::BigCore));

    // Bullets
    RTTR_REGISTER_COMPONENT(Gradius::BaseBullet, "BaseBullet", Helios::Entity&, Helios::ComponentID)
        RTTR_PROPERTY("Bullet Speed", Gradius::BaseBullet::m_bulletSpeed)
        RTTR_PROPERTY("Damage", Gradius::BaseBullet::m_damage)
        RTTR_PROPERTY("Direction", Gradius::BaseBullet::m_direction);

    RTTR_REGISTER_COMPONENT(Gradius::DefaultBullet, "Default Bullet", Helios::Entity&, const Helios::ComponentID&);
    RTTR_REGISTER_COMPONENT(Gradius::MissileBullet, "Missile Bullet", Helios::Entity&, const Helios::ComponentID&);
    RTTR_REGISTER_COMPONENT(Gradius::DoubleBullet, "Double Bullet", Helios::Entity&, const Helios::ComponentID&);
    RTTR_REGISTER_COMPONENT(Gradius::BigCoreBullet, "BigCore Bullet", Helios::Entity&, const Helios::ComponentID&);
    RTTR_REGISTER_COMPONENT(Gradius::LaserBullet, "Laser Bullet", Helios::Entity&, const Helios::ComponentID&);

    //Killbox
    RTTR_REGISTER_COMPONENT(Gradius::KillBox, "KillBox", Helios::Entity&, Helios::ComponentID);

    RTTR_REGISTER_COMPONENT(Gradius::PlayerLivesUI, "Player Lives UI", Helios::Entity&, Helios::ComponentID);

    RTTR_REGISTER_COMPONENT(Gradius::PlayerShieldComponent, "PlayerShieldComponent", Helios::Entity&, Helios::ComponentID)
		RTTR_PROPERTY("Max shield health", Gradius::PlayerShieldComponent::m_maxShieldHealth)
		RTTR_PROPERTY("Bullet damage to shield", Gradius::PlayerShieldComponent::m_bulletDamageToShield)
		RTTR_PROPERTY("Direct damage to shield", Gradius::PlayerShieldComponent::m_directDamageToShield);
}