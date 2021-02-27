// EmbedingLua.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

extern "C"
{
#include "Lua/include/lua.h"
#include "Lua/include/lauxlib.h"
#include "Lua/include/lualib.h"
}
int LUA_f(LuaState * lua)
{
	return 0;
}

int main()
{
	std::string cmd = "a = 7 + 11k";

	lua_State* L = luaL_newstate();

	int r = luaL_dostring(L, cmd.c_str());

	if(r== LUA_OK)
	{
		lua_getglobal(L, "a");
		if(lua_isnumber(L,-1))
		{
			float a = (float)lua_tonumber(L, -1);
			std::cout << a << std::endl;
		}
	}
	else
	{
		std::string errormsg = lua_tostring(L, -1);
		std::cout << errormsg << std::endl;
	}
    std::cout << "Hello World!\n";
	lua_close(L);
	
	L = luaL_newstate();
	lua_close(L);
	return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
