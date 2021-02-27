#include "pch.h"
#ifdef _DEBUG
#include "hepch.h"
#include "CppUnitTest.h"
#include "Utils/Serializer.h"
#include "Components/Colliders/CircleCollider.h"
#include "Components/Colliders/QuadCollider.h"
#include "Utils/TriangleMath.h"
#include "Core/EntityComponent/EntityManager.h"
#include "Core/EntityComponent/Entity.h"
#include "Core/CoreRegistration.h"
#include "DirectXMath.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
namespace UnitTesting
{

    struct SerializationSubTest
    {
        RTTR_ENABLE()
    public:
        SerializationSubTest(int i = 0) : i(i) { };
        int i = 5757;
    };

    struct SerializationTest
    {
        RTTR_ENABLE()
    public:
        int x = 5;
        float y = 69.f;
        bool z = false;
        std::string w = "Hello World!";
        std::vector<SerializationSubTest> a =
        {
            SerializationSubTest{1}, SerializationSubTest{2}, SerializationSubTest()
        };
    };

    RTTR_REGISTRATION
    {
        RTTR_REGISTER_CLASS(SerializationTest, "SerializationTest")
        RTTR_PROPERTY("X", SerializationTest::x)
        RTTR_PROPERTY("Y", SerializationTest::y)
        RTTR_PROPERTY("Z", SerializationTest::z)
        RTTR_PROPERTY("W", SerializationTest::w)
        RTTR_PROPERTY("A", SerializationTest::a);

        RTTR_REGISTER_CLASS(SerializationSubTest, "SubSerializationTest", int)
        RTTR_PROPERTY("I", SerializationSubTest::i);
    }


        TEST_CLASS(UnitTesting)
    {
    public:
        TEST_METHOD(TestSerializationAndDeserialization)
        {
            constexpr std::size_t toSerialize = 10000;
            std::vector<SerializationTest> tests(toSerialize);

            std::vector<std::string> serialized;
            serialized.reserve(toSerialize);
            for (auto t : tests)
            {
                t.x = 10;
                t.y = 999.f;
                t.z = true;
                serialized.push_back(Helios::Serializer::Serialize(t));
            }

            Assert::IsTrue((toSerialize == serialized.size()), L"Serialized data not same size!");
            std::vector<SerializationTest> serializedTests;
            for (auto& s : serialized)
            {
                SerializationTest test;
                Helios::Deserializer::Deserialize(s, test);

                Assert::IsTrue((test.x == 10));
                Assert::IsTrue((test.y == 999.f));
                Assert::IsTrue((test.z == true));
                serializedTests.push_back(test);
            }
            Assert::IsTrue((serializedTests.size() == toSerialize), L"Deserialized data not same size!");
        }

        TEST_METHOD(TestEntityCreation)
        {
            constexpr int toCreate = 10000;
            std::vector<Helios::Entity*> entities;
            entities.reserve(toCreate);

            for (int i = 0; i < toCreate; i++)
            {
                entities.push_back(Helios::EntityManager::GetInstance().CreateEntity());
            }
        }

        TEST_METHOD(MultipleCircleToCircleCollision)
        {
            constexpr std::size_t collisionsToTest = 1000;
            std::vector<Helios::Entity*> entities;
            entities.reserve(collisionsToTest);

            for (std::size_t i = 0; i < collisionsToTest; ++i)
            {
                const int row = int(i / (collisionsToTest / 2.f));
                const int column = i % int(collisionsToTest / 2.f);

                Helios::Entity* entity = Helios::EntityManager::GetInstance().CreateEntity();
                entity->GetTransform().SetPosition({ column, row, 0.f });
                auto circle = entity->AddComponent<Helios::CircleCollider>();
                circle->SetRadius(16);
                entities.push_back(entity);
            }

            for (auto e : entities)
            {
                const auto circle1 = e->GetComponent<Helios::CircleCollider>();
                circle1->Render();
                for (auto e1 : entities)
                {
                    if (e->GetID() == e1->GetID())
                    {
                        continue;
                    }

                    const auto circle2 = e->GetComponent<Helios::CircleCollider>();
                    circle2->Render();

                    if (DoCirclesIntersect(*circle1, *circle2))
                    {

                    }
                }
            }
        }

        TEST_METHOD(MultipleCircleToQuadCollision)
        {
            constexpr std::size_t collisionsToTest = 100;
            constexpr auto numCircles = std::size_t(collisionsToTest / 2.f);
            constexpr auto numQuads = std::size_t(collisionsToTest / 2.f);

            std::vector<Helios::Entity*> circleEntities;
            std::vector<Helios::Entity*> quadEntities;
            circleEntities.reserve(numCircles);
            quadEntities.reserve(numQuads);

            for (std::size_t i = 0; i < numCircles; ++i)
            {
                const int row = int(i / (numCircles / 2.f));
                const int column = i % int(numCircles / 2.f);
                Helios::Entity* entity = Helios::EntityManager::GetInstance().CreateEntity();
                entity->GetTransform().SetPosition({ column, row, 0.f });
                auto circle = entity->AddComponent<Helios::CircleCollider>();
                circle->SetRadius(16);

                circleEntities.push_back(entity);
            }

            for (std::size_t i = 0; i < numQuads; ++i)
            {
                const int row = int(i / (numQuads / 2.f));
                const int column = i % int(numQuads / 2.f);
                Helios::Entity* entity = Helios::EntityManager::GetInstance().CreateEntity();
                entity->GetTransform().SetPosition({ column, row, 0.f });
                auto quad = entity->AddComponent<Helios::QuadCollider>();
                quad->SetSize(16, 16);

                quadEntities.push_back(entity);
            }

            constexpr int iterators = 10;
            int i = 0;
            bool noCollisionHappen = false;
            do 
            {
                for (auto e : circleEntities)
                {
                    Vec3 pos = e->GetTransform().GetPosition();
                    pos.x += 1;
                    e->GetTransform().SetPosition(pos);

                    const auto c = e->GetComponent<Helios::CircleCollider>();
                    c->Render();
                    for (auto e2 : quadEntities)
                    {
                        pos = e2->GetTransform().GetPosition();
                        pos.x += 1;
                        e2->GetTransform().SetPosition(pos);

                        const auto q = e2->GetComponent<Helios::QuadCollider>();
                        q->Render();

                        if (q->OverlapsWith(*c))
                        {
                            noCollisionHappen = true;
                        }
                    }
                }
                i++;
            } while(i < iterators);
            Assert::IsTrue(noCollisionHappen);
        }

        TEST_METHOD(CircleToCircleCollision)
        {
            Helios::Entity* entity1 = Helios::EntityManager::GetInstance().CreateEntity("Circle1");
            Helios::CircleCollider* circle1 = entity1->AddComponent<Helios::CircleCollider>();

            Helios::Entity* entity2 = Helios::EntityManager::GetInstance().CreateEntity("Circle2");
            Helios::CircleCollider* circle2 = entity2->AddComponent<Helios::CircleCollider>();

            entity1->GetTransform().SetPosition(glm::vec3(0, 0, 0));
            entity2->GetTransform().SetPosition(glm::vec3(8, 0, 0));

            circle1->SetRadius(16);
            circle2->SetRadius(32);

            //This sets their position
            circle1->Render();
            circle2->Render();

            bool overlaps = DoCirclesIntersect(*circle1, *circle2);
            bool expectedResult = true;

            Assert::AreEqual(overlaps, expectedResult);
        }

        TEST_METHOD(CircleToBoxCollision)
        {
            Helios::Entity* entity1 = Helios::EntityManager::GetInstance().CreateEntity("Circle1");
            Helios::CircleCollider* circle1 = entity1->AddComponent<Helios::CircleCollider>();

            Helios::Entity* entity2 = Helios::EntityManager::GetInstance().CreateEntity("Box1");
            Helios::QuadCollider* box1 = entity2->AddComponent<Helios::QuadCollider>();

            entity1->GetTransform().SetPosition(glm::vec3(32, 32, 0));
            entity2->GetTransform().SetPosition(glm::vec3(4, 0, 0));

            circle1->SetRadius(16);
            box1->SetSize(8, 8);

            //This sets their position
            circle1->Render();
            box1->Render();

            bool triangle1 = CircleIntersectsWithTriangle(circle1->GetVertices()[0], circle1->GetRadius(), &box1->GetPolygons()[0]);
            bool triangle2 = CircleIntersectsWithTriangle(circle1->GetVertices()[0], circle1->GetRadius(), &box1->GetPolygons()[1]);
            bool overlaps = (triangle1 || triangle2);
            bool expectedResult = false;

            Assert::AreEqual(overlaps, expectedResult);
        }

        TEST_METHOD(BoxToBoxCollision)
        {
            Helios::Entity* entity1 = Helios::EntityManager::GetInstance().CreateEntity("Box1");
            Helios::QuadCollider* box1 = entity1->AddComponent<Helios::QuadCollider>();

            Helios::Entity* entity2 = Helios::EntityManager::GetInstance().CreateEntity("Box2");
            Helios::QuadCollider* box2 = entity2->AddComponent<Helios::QuadCollider>();

            entity1->GetTransform().SetPosition(glm::vec3(0, 0, 0));
            entity2->GetTransform().SetPosition(glm::vec3(32, 0, 0));

            box1->SetSize(8, 8);
            box2->SetSize(8, 8);

            //This sets their position
            box1->Render();
            box2->Render();

            bool overlaps1 = TriTri2D(&box1->GetPolygons()[0], &box2->GetPolygons()[0]);
            bool overlaps2 = TriTri2D(&box1->GetPolygons()[1], &box2->GetPolygons()[0]);
            bool overlaps3 = TriTri2D(&box1->GetPolygons()[0], &box2->GetPolygons()[1]);
            bool overlaps4 = TriTri2D(&box1->GetPolygons()[1], &box2->GetPolygons()[1]);
            bool expectedResult = false;
            bool overlaps = false;
            if (overlaps1 || overlaps2 || overlaps3 || overlaps4)
            {
                overlaps = true;
            }

            Assert::AreEqual(overlaps, expectedResult);

        }
    };

    TEST_CLASS(TransformTesting)
    {
    public:
        TEST_METHOD(TransformPosition)
        {
            Helios::Entity* entity = Helios::EntityManager::GetInstance().CreateEntity("Parent");
            entity->GetTransform().SetPosition(glm::vec3(90, 125, 10));

            const glm::vec3 expected = glm::vec3(90, 125, 10);
            const glm::vec3 result = entity->GetTransform().GetPosition();

            Assert::IsTrue(CheckVectors(expected, result));
        }

        TEST_METHOD(TransformChildingPosition)
        {
            Helios::Entity* parent = Helios::EntityManager::GetInstance().CreateEntity("Parent");
            parent->GetTransform().SetPosition(glm::vec3(0, 0, 0));

            Helios::Entity* child = Helios::EntityManager::GetInstance().CreateEntity("Child");
            child->GetTransform().SetPosition(glm::vec3(50, 0, 0));

            child->GetTransform().SetParent(&parent->GetTransform());

            glm::vec3 temp = parent->GetTransform().GetPosition();
            temp += glm::vec3(100, 0, 0);

            parent->GetTransform().SetPosition(temp);

            const glm::vec3 expected = glm::vec3(150, 0, 0);
            const glm::vec3 result = child->GetTransform().GetPosition();

            Assert::IsTrue(CheckVectors(expected, result));
        }

        TEST_METHOD(TransformRotation)
        {
            Helios::Entity* entity = Helios::EntityManager::GetInstance().CreateEntity();
            entity->GetTransform().SetRotation({ -45.f, -90.f, 180.f });

            const glm::quat expected = glm::quat(0.2705981f, 0.6532815f, -0.2705981f, 0.6532815f);
            const glm::quat result = entity->GetTransform().GetRotation();

            Assert::IsTrue(CheckQuaternions(expected, result));
        }

        TEST_METHOD(TransformRotationEuler)
        {
            Helios::Entity* entity = Helios::EntityManager::GetInstance().CreateEntity();
            entity->GetTransform().SetRotation({ -45.f, 0.f, 90.f });

            const glm::vec3 expected = glm::vec3(-45.f, 0.f, 90.f);
            const glm::vec3 result = entity->GetTransform().GetRotationInEuler();

            Assert::IsTrue(CheckVectors(expected, result));
        }

        TEST_METHOD(TransformWorldRotationEuler)
        {
            Helios::Entity* parent = Helios::EntityManager::GetInstance().CreateEntity("Parent");
            Helios::Entity* entity = Helios::EntityManager::GetInstance().CreateEntity("Parent");
            entity->GetTransform().SetParent(&parent->GetTransform());
            entity->GetTransform().SetRotation({ 45.f, 0.f, 0.f });

            const glm::vec3 expected = glm::vec3(45.f, 0.f, 0.f);
            const glm::vec3 result = entity->GetTransform().GetRotationInEuler();

            Assert::IsTrue(CheckVectors(expected, result));
        }

        TEST_METHOD(TransformChildingRotationPosition)
        {
            Helios::Entity* parent = Helios::EntityManager::GetInstance().CreateEntity("Parent");
            parent->GetTransform().SetPosition({ 500.f, 300.f, 0.f });
            parent->GetTransform().SetRotation({ 90.f, 0.f, 0.f });

            Helios::Entity* child = Helios::EntityManager::GetInstance().CreateEntity("Child");
            child->GetTransform().SetLocalPosition({ 50.f, 50.f, 0 });
            child->GetTransform().SetParent(&parent->GetTransform());

            const glm::vec3 expected = glm::vec3(550.f, 300.f, 50.f);
            const glm::vec3 result = child->GetTransform().GetPosition();

            Assert::IsTrue(CheckVectors(expected, result));
        }

        bool CheckVectors(const Vec3& a, const Vec3& b) const
        {
            return abs(a.x - b.x) < 0.1f &&
                abs(a.y - b.y) < 0.1f &&
                abs(a.z - b.z) < 0.1f;
        }

        bool CheckQuaternions(const Quaternion& a, const Quaternion& b) const
        {
            return abs(a.x - b.x) < 0.1f &&
                abs(a.y - b.y) < 0.1f &&
                abs(a.z - b.z) < 0.1f &&
                abs(a.w - b.w) < 0.1f;
        }

        bool CheckMatrices(const Mat4& a, const Mat4& b) const
        {
            for (int i = 0; i < Mat4::length(); ++i)
            {
                for (int j = 0; j < Vec4::length(); ++j)
                {
                    if (abs(a[i][j] - b[i][j]) > 0.1f)
                        return false;
                }
            }
            return true;
        }
    };
}
#endif
