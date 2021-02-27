#include "hepch.h"
#include "Components/LuaComponent.h"
#include <fstream>
namespace Helios
{

	LuaComponent::LuaComponent(Entity& owner, const ComponentID& a_id) : ScriptableComponent(owner, a_id)
	{
		for (auto& type : rttr::type::get<Component>().get_derived_classes())
		{
			HE_CORE_INFO("Class Name: {0}", type.get_name().to_string());
			for (auto& method : type.get_methods())
			{
				HE_CORE_INFO("Method Name: {0}", method.get_name().to_string());
			}
		}
	}

	bool LuaComponent::DoesLuaCompile(lua_State* a_luaState, std::string a_file, bool a_isInPlayMode)
	{
		luaL_openlibs(a_luaState);

		lua_pushlightuserdata(a_luaState, this);
		lua_setglobal(a_luaState, "this");
		lua_register(a_luaState, "Log", LUA_Log);

		lua_pushcfunction(a_luaState, LUA_ExposeNumber);
		lua_setglobal(a_luaState, ("ExposeNumber"));

		if (!m_filePath.empty())
		{
			int result = 0;
			if(a_isInPlayMode)
			{
				luaL_dostring(a_luaState,a_file.c_str());
			}
			else
			{
				result = luaL_dofile(a_luaState, a_file.c_str());
			}
			if (result != LUA_OK)
			{
				std::string errormsg = lua_tostring(a_luaState, -1);
				HE_CORE_ERROR("[LUA ERROR] {0}", errormsg);
				return false;
			}
			else
			{
				return true;
			}
		}
		return false;

	}


	void LuaComponent::Start()
	{
		std::ifstream stream;
		stream.open(m_filePath);
		while (stream)
		{
			std::getline(stream, m_luaString);
		}
		
		L = luaL_newstate();

		if (DoesLuaCompile(L, m_filePath))
		{

			lua_getglobal(L, "Start");
			if (lua_isfunction(L, -1))
			{
				lua_pcall(L, 0, 0, 0);
			}
		}
		lua_close(L);
	}

	void LuaComponent::Update(float dt)
	{
		L = luaL_newstate();
		if (DoesLuaCompile(L, m_filePath))
		{
			lua_getglobal(L, "Update");
			if (lua_isfunction(L, -1))
			{
				lua_pushnumber(L, dt);
				lua_pcall(L, 1, 0, 0);
			}
		}
		lua_close(L);
	}

	void LuaComponent::GetExposedVariables()
	{
		L = luaL_newstate();
		lua_register(L, "ExposeNumber", LUA_ExposeNumber);

		if (DoesLuaCompile(L, m_filePath))
		{
		}
		lua_close(L);
	}

	void LuaComponent::SetFilePath(std::string a_newPath)
	{
		m_filePath = a_newPath;
	}

	const std::map<std::string, rttr::variant>& LuaComponent::GetExposedLuaVariables()
	{
		return m_exposedLuaVariables;
	}

	void LuaComponent::SetExposedVariable(std::string a_name, rttr::variant a_newValue)
	{
		auto it = m_exposedLuaVariables.find(a_name);
		if (it != m_exposedLuaVariables.end())
		{
			m_exposedLuaVariables[a_name] = a_newValue;
		}
	}

	void LuaComponent::AddExposedVariable(std::string a_name, rttr::variant a_type)
	{
		m_exposedLuaVariables[a_name] = a_type;
	}

	int LuaComponent::LUA_ExposeNumber(lua_State* a_l)
	{
		std::string name = std::string(lua_tostring(a_l, -2));
		double n = float(lua_tonumber(a_l, -1));
		lua_getglobal(a_l, "this");
		LuaComponent* obj = (LuaComponent*)lua_topointer(a_l, -1);

		if (n == (int)n) {
			// n is an int
			obj->AddExposedVariable(name, int(n));

		}
		else {
			// n is a float
			obj->AddExposedVariable(name, n);

		}
		return 0;
	}

	int LuaComponent::LUA_ExposeBool(lua_State* l)
	{
		std::string name = std::string(lua_tostring(l, -2));
		bool b = lua_toboolean(l, -1);
		lua_getglobal(l, "this");
		LuaComponent* obj = (LuaComponent*)lua_topointer(l, -1);
		obj->AddExposedVariable(name, b);
		return 0;

	}

	int LuaComponent::LUA_ExposeString(lua_State* a_l)
	{
		std::string name = std::string(lua_tostring(a_l, -2));
		std::string string = std::string(lua_tostring(a_l, -1));
		lua_getglobal(a_l, "this");
		LuaComponent* obj = (LuaComponent*)lua_topointer(a_l, -1);
		obj->AddExposedVariable(name, string);
		return 0;
	}

	int LuaComponent::LUA_Log(lua_State* a_l)
	{
		const std::string logmessage = std::string(lua_tostring(a_l, -1));
		HE_CORE_INFO(logmessage);
		return 0;
	}

	void LuaComponent::PushAllChangedVarsToLua(lua_State* a_l)
	{
		for (auto p : m_exposedLuaVariables)
		{
			if (p.second.is_type<int>() || p.second.is_type<double>())
			{
				lua_Number n;

				if (p.second.is_type<double>())
				{
					n = lua_Number(p.second.get_value<double>());
				}
				else
				{
					n = lua_Number(p.second.get_value<int>());
				}
				lua_pushnumber(a_l, n);
			}
			lua_setglobal(a_l, p.first.c_str());
		}
	}
}
