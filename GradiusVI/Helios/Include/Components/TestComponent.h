#pragma once
#include "Components/ScriptableComponent.h"
#include "Core/Prefab.h"

namespace Helios
{
	class Entity;

    struct TestClass
    {
        RTTR_ENABLE()
    public:
        TestClass(int x = 0) : x(x) { };
        int x = 0;
    	float y = 69.69f;
    	bool z = false;
    	std::string w = "Hello world!";
    };
	
	class TestComponent : public ScriptableComponent
	{
		RTTR_ENABLE(Component)
	public:
		TestComponent(Entity& a_owner, const ComponentID& a_id);
		virtual ~TestComponent() = default;

        void Start() override;
		void Update(float) override;

		int myInt = 5;
		float myFloat = 69.93f;
		bool myBool = true;
		std::string myString = "Hallo";
		std::shared_ptr<Prefab> myPrefab;

        std::vector<int> myArray = 
        {
           1, 2, 3
        };

        std::vector<TestClass> myArrayClasses =
        {
            { 1 }, { 2, }, { 3 }
        };
	};
}
