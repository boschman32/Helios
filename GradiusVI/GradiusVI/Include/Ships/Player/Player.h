#pragma once
#include "Ships/BaseShip/BaseShip.h"
#include "Weapons/WeaponComponent.h"
#include "PowerUp/PowerUpMeter.h"
#include "Core/InputManager.h"
#include "Components/Colliders/QuadCollider.h"
#include "Utils/Timer.h"

namespace Gradius
{
    class PlayerShieldComponent;

    constexpr int m_maxLives = 3;
    constexpr float m_respawnTime = 3.f;
    constexpr float m_invulnerableTime = 3.f;

    enum class EPlayerState
    {
        Alive,
        Dead,
        Invulnerable
    };

    class Player : public BaseShip
    {
        RTTR_ENABLE(BaseShip, Component)
    public:
        Player(Helios::Entity& a_owner, Helios::ComponentID a_id);

        void Start() override;
        void Update(float deltaTime) override;
        void OnOverlapBegin(Helios::Collider* a_col) override;
    	void DamageShip(int) override;

        void AddShipSpeed(float a_speed);
    	void EnableShield() const;
        EPlayerState GetPlayerState() const { return m_playerState; };

        std::vector<WeaponComponent*> m_weaponComponents;

        static std::function<void(int a_lives)> s_UpdateLivesCallback;
        static int GetMaxLives() { return m_maxLives; };

    private:
        void Move(float a_deltaTime);
        void FireWeapons();
        void ActivatePowerUp();
        void SetInput();
        void RespawnShip();
        void DestroyShip() override;

    	Vec3 ClampInEdge(const Vec3& a_pos) const;

        PowerUpMeter* m_powerupMeter = nullptr;
        Helios::QuadCollider* m_collider = nullptr;
        PlayerShieldComponent* m_shield;

        std::unique_ptr<Helios::Controller> controller = nullptr;

        EPlayerState m_playerState;

        Helios::Timer<float> m_respawnTimer;
        Helios::Timer<float> m_invulnerableTimer;
        int m_blinkCount = 0;
        int m_lives = 0;
        bool m_isAlive = true;
        glm::vec2 m_velocity;
        float m_acceleration;
    	
    	float m_bankingAngle = 15.f;
    	float m_bankingSpeed = 35.f;
    	bool m_isBanking = false;

        //BoundingBox
        float m_edgeLeft = -380.f;
        float m_edgeRight = 1400.f;
        float m_edgeTop = -80.f;
        float m_edgeBottom = 900.f;
    	RTTR_REGISTRATION_FRIEND
    };
};