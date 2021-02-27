#include "hepch.h"
#include "Core/InputManager.h"

namespace Helios
{
	std::vector < std::unique_ptr<Controller>> InputManager::m_controllers = std::vector<std::unique_ptr<Controller>>();

	void Axis::SetName(const std::string& a_name)
	{
		m_name = a_name;
	}

	void Button::SetJoystick(unsigned int a_joystick)
	{
		m_joystick = a_joystick;
	}

	void Button::AddKey(const Key& a_key)
	{
		m_keyboardKeys.push_back(a_key);
	}

	void Button::AddJoystickButton(unsigned int a_joystickButton)
	{
		m_joystickButtons.push_back(a_joystickButton);
	}

	void Button::SetName(const std::string& a_name)
	{
		m_name = a_name;
	}

	const std::string& Button::GetName() const
	{
		return m_name;
	}

	bool Button::IsAnyKeDown() const
	{
		return m_isPressed;
	}

	void Button::OnKeyPressed(KeyEventArgs& e)
	{
		const KeyCode::Key pressedKey = e.Key;
		for(auto &k : m_keyboardKeys)
		{
			if (k == pressedKey)
			{
				if (!m_isPressed)
				{
					m_hasBeenDown = true;
				}
				m_isPressed = true;
			}
		}
	}

	void Button::OnKeyReleased(KeyEventArgs& e)
	{
		const KeyCode::Key pressedKey = e.Key;
		for (auto &k : m_keyboardKeys)
		{
			if (k == pressedKey)
			{
				m_isPressed = false;
				m_hasBeenDown = false;
			}
		}
	}

	void Controller::AddButton(const Button& a_button)
	{
		m_buttons.push_back(a_button);
	}

	bool Controller::GetButtonOnce(std::string const& a_buttonName)
	{
		if (m_buttons.empty())
		{
			return false;
		}
		for (auto &b : m_buttons)
		{
			if (b.GetName() == a_buttonName)
			{
				if (b.HasBeenDown())
				{
					b.ResetHasBeenDown();
					return true;
				}
			}
		}

		return false;
	}

	bool Button::HasBeenDown() const 
	{
		return m_hasBeenDown;
	}

	void Button::ResetHasBeenDown()
	{
		m_hasBeenDown = false;
	}
	
	void Controller::AddAxis(const Axis& a_axis)
	{
		m_axises.push_back(a_axis);
	}

	bool Controller::GetButtonDown(const std::string& a_buttonName)
	{
		if (m_buttons.empty())
		{
			return false;
		}
		for (auto &b : m_buttons)
		{
			if (b.GetName() == a_buttonName)
			{
				if (b.IsAnyKeDown())
				{
					return true;
				}
			}
		}

		return false;
	}

	bool Controller::GetButtonUp(const std::string& a_buttonName)
	{
		if (m_buttons.empty())
		{
			return false;
		}
		for (auto &b : m_buttons)
		{
			if (b.GetName() == a_buttonName)
			{
				if (!b.IsAnyKeDown())
				{
					return true;
				}
			}
		}

		return false;
	}



	float Controller::GetAxis(const std::string& a_axisName)
	{
		if (m_axises.empty())
		{
			return 0.0f;
		}

		for (const auto &a : m_axises)
		{
			if (a.m_name == a_axisName)
			{
				return a.m_value;
			}
		}
		return 0.0f;
	}

	Helios::Controller* Helios::InputManager::GetController(int a_id)
	{
		if (!m_controllers.empty())
		{
			if (a_id >= 0 && a_id < m_controllers.size())
			{
				return m_controllers[a_id].get();
			}
		}
		return nullptr;

	}

	void InputManager::AddController(std::unique_ptr<Controller> a_controller)
	{
		m_controllers.emplace_back(std::move(a_controller));
	}

	const std::vector<std::unique_ptr<Controller>>& InputManager::GetAllControllers()
	{
		return m_controllers;
	}
}
