#pragma once

#include "Renderer/KeyCodes.h"
#include "Renderer/InputEvents.h"
using Key = KeyCode::Key;

namespace Helios
{
	class Axis
	{
		void SetName(const std::string& a_name);
	public:
		float m_value = 0.0f;
		std::string m_name;
	};

	class Button
	{
		std::vector<Key> m_keyboardKeys;
		std::vector<unsigned int> m_joystickButtons;
		std::string m_name;
		unsigned int m_joystick = 0;
		bool m_isPressed = false;
		bool m_hasBeenDown = false;

	public:
		void SetJoystick(unsigned int a_joystick);
		void AddKey(const Key& a_key);
		void AddJoystickButton(unsigned int a_joystickButton);
		void SetName(const std::string& a_name);
		const std::string& GetName() const;
		bool IsAnyKeDown() const;
		bool HasBeenDown() const;
		void ResetHasBeenDown();

		void OnKeyPressed(KeyEventArgs& e);
		void OnKeyReleased(KeyEventArgs& e);
	};

	class Controller
	{
	public:
		void AddButton(const Button& a_button);
		void AddAxis(const Axis& a_axis);
		bool GetButtonDown(const std::string& a_buttonName);
		bool GetButtonUp(const std::string& a_buttonName);
		bool GetButtonOnce(const std::string& a_buttonName);
		float GetAxis(const std::string& a_axisName);

		std::vector<Button> m_buttons;
		std::vector<Axis> m_axises;
	};

	class InputManager
	{
		static std::vector <std::unique_ptr<Controller>> m_controllers;
	public:
		static Controller* GetController(int a_id);
		static void AddController(std::unique_ptr<Controller> a_controller);
		static const std::vector<std::unique_ptr<Controller>>& GetAllControllers();
	};
}



