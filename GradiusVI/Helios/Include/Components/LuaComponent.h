#pragma once
#include "Components/ScriptableComponent.h"
#include <map>
#include <string>
#include <rttr/registration.h>
#include <lua.hpp>
namespace Helios
{
	class LuaComponent : public ScriptableComponent
	{
		RTTR_ENABLE(Component)
		RTTR_REGISTRATION_FRIEND
	public:
		LuaComponent(Entity& a_owner, const ComponentID& a_id);
		bool DoesLuaCompile(lua_State* a_luaState, std::string a_file, bool a_isInPlayMode = false);

		void Start() override;
		void Update(float) override;

		void GetExposedVariables();
		
		void SetFilePath(std::string a_newPath);
		
		int LUA_SetClientVar(lua_State* a_l, std::string a_varName, rttr::variant a_value);

		const std::map<std::string, rttr::variant>& GetExposedLuaVariables();
		void SetExposedVariable(std::string a_name, rttr::variant a_newValue);
	private:

		void AddExposedVariable(std::string a_name, rttr::variant a_type);
		
		static int LUA_ExposeNumber(lua_State* a_l);
		static int LUA_ExposeBool(lua_State* a_l);
		static int LUA_ExposeString(lua_State* a_l);

		static int LUA_Log(lua_State* a_l);
		char * CharReplace(char* a_str, char a_find, char a_replace);

		void PushAllChangedVarsToLua(lua_State* a_l);
		
		std::map<std::string, rttr::variant> m_exposedLuaVariables;
		lua_State * L = nullptr;
		std::string m_filePath;
		std::string m_luaString;
	};
}

