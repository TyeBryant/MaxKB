#include "bot.h"
#include "time.h"
#include "kf/kf_log.h"

extern "C"
{
	BotInterface BOT_API *CreateBot()
	{
		return new LuaBot();
	}
}

int luaPrint(lua_State *l)
{
	const char *temp=lua_tostring(l, -1);
	kf_log("Lua:"<<temp);
	return 0;
}

LuaBot::LuaBot()
{
	kf::LogSystem::getDefault().addCout();
	m_rand(time(0)+(int)this);
	m_lua = luaL_newstate();
	luaL_openlibs(m_lua);
	lua_register(m_lua, "print", luaPrint);
}

LuaBot::~LuaBot()
{
}

void LuaBot::init(const BotInitialData &initialData, BotAttributes &attrib)
{
	attrib.health=1.0;
	attrib.motor=1.0;
	attrib.weaponSpeed=1.0;
	attrib.weaponStrength=1.0;
	std::string script = "data/bots/luabot.lua";
	for (int i = 0; i < initialData.properties.size(); ++i)
	{
		if (initialData.properties[i].first == "script")
		{
			script = initialData.properties[i].second;
		}
	}
	luaL_dofile(m_lua, script.c_str());

	lua_getglobal(m_lua, "init");
	if(lua_isfunction(m_lua, -1))
	{
		lua_newtable(m_lua);
		lua_pushstring(m_lua, initialData.mapName.c_str());
		lua_setfield(m_lua, -2, "mapName");
		lua_pushnumber(m_lua, initialData.scanFOV);
		lua_setfield(m_lua, -2, "fov");
		lua_pushnumber(m_lua, initialData.mapData.width);
		lua_setfield(m_lua, -2, "width");
		lua_pushnumber(m_lua, initialData.mapData.height);
		lua_setfield(m_lua, -2, "height");
		
		lua_newtable(m_lua);
		for (int i = 0; i < initialData.properties.size(); ++i)
		{
			lua_pushstring(m_lua, initialData.properties[i].second.c_str());
			lua_setfield(m_lua, -2, initialData.properties[i].first.c_str());
		}
		lua_setfield(m_lua, -2, "properties");


		lua_call(m_lua, 1, 1);
		
		if(lua_istable(m_lua,-1))
		{
			lua_getfield(m_lua, -1, "health");
			attrib.health = lua_tonumber(m_lua, -1);
			lua_pop(m_lua, 1);
			lua_getfield(m_lua, -1, "motor");
			attrib.motor = lua_tonumber(m_lua, -1);
			lua_pop(m_lua, 1);
			lua_getfield(m_lua, -1, "weaponSpeed");
			attrib.weaponSpeed = lua_tonumber(m_lua, -1);
			lua_pop(m_lua, 1);
			lua_getfield(m_lua, -1, "weaponStrength");
			attrib.weaponStrength = lua_tonumber(m_lua, -1);
			lua_pop(m_lua, 1);
		}
	}
	lua_settop(m_lua, 0);
}

void LuaBot::update(const BotInput &input, BotOutput &output)
{
	int stack = lua_gettop(m_lua);
	lua_getglobal(m_lua, "step");
	if(lua_isfunction(m_lua, -1))
	{
		lua_newtable(m_lua);
		lua_pushnumber(m_lua, input.health);
		lua_setfield(m_lua, -2, "health");
		lua_pushnumber(m_lua, input.healthMax);
		lua_setfield(m_lua, -2, "healthMax");
		lua_pushnumber(m_lua, input.bulletSpeed);
		lua_setfield(m_lua, -2, "bulletSpeed");
		lua_newtable(m_lua);
		lua_pushnumber(m_lua, input.position.x);
		lua_setfield(m_lua, -2, "x");
		lua_pushnumber(m_lua, input.position.y);
		lua_setfield(m_lua, -2, "y");
		lua_setfield(m_lua, -2, "position");
		lua_newtable(m_lua);
		lua_pushnumber(m_lua, input.velocity.x);
		lua_setfield(m_lua, -2, "x");
		lua_pushnumber(m_lua, input.velocity.y);
		lua_setfield(m_lua, -2, "y");
		lua_setfield(m_lua, -2, "velocity");
		
		lua_newtable(m_lua);
		for(int i=0;i<input.scanResult.size();i++)
		{
			lua_newtable(m_lua);
			lua_pushstring(m_lua, input.scanResult[i].name.c_str());
			lua_setfield(m_lua, -2, "name");
			if(input.scanResult[i].type==VisibleThing::e_bullet)
			{
				lua_pushstring(m_lua, "bullet");
			}
			else if(input.scanResult[i].type==VisibleThing::e_robot)
			{
				lua_pushstring(m_lua, "robot");
			}
			else
			{
				lua_pushstring(m_lua, "unknown");
			}
			lua_setfield(m_lua, -2, "type");		
			lua_newtable(m_lua);
			lua_pushnumber(m_lua, input.scanResult[i].position.x);
			lua_setfield(m_lua, -2, "x");
			lua_pushnumber(m_lua, input.scanResult[i].position.y);
			lua_setfield(m_lua, -2, "y");
			lua_setfield(m_lua, -2, "position");
			lua_rawseti(m_lua,-2, i+1);
		}
		lua_setfield(m_lua, -2, "scanResult");
		
		lua_newtable(m_lua);
		lua_pushnumber(m_lua, output.motor);
		lua_setfield(m_lua, -2, "motor");
		lua_pushstring(m_lua, output.action==BotOutput::shoot?"shoot":"scan");
		lua_setfield(m_lua, -2, "action");
		lua_newtable(m_lua);
		lua_pushnumber(m_lua, output.lookDirection.x);
		lua_setfield(m_lua, -2, "x");
		lua_pushnumber(m_lua, output.lookDirection.y);
		lua_setfield(m_lua, -2, "y");
		lua_setfield(m_lua, -2, "lookDirection");
		lua_newtable(m_lua);
		lua_pushnumber(m_lua, output.moveDirection.x);
		lua_setfield(m_lua, -2, "x");
		lua_pushnumber(m_lua, output.moveDirection.y);
		lua_setfield(m_lua, -2, "y");
		lua_setfield(m_lua, -2, "moveDirection");


		lua_call(m_lua, 2, 1);
		
		if(lua_istable(m_lua,-1))
		{
			lua_getfield(m_lua, -1, "motor");
			output.motor = lua_tonumber(m_lua, -1);
			lua_pop(m_lua, 1);
			lua_getfield(m_lua, -1, "lookDirection");
			lua_getfield(m_lua, -1, "x");
			output.lookDirection.x = lua_tonumber(m_lua, -1);
			lua_pop(m_lua, 1);
			lua_getfield(m_lua, -1, "y");
			output.lookDirection.y = lua_tonumber(m_lua, -1);
			lua_pop(m_lua, 1);
			lua_pop(m_lua, 1);
			lua_getfield(m_lua, -1, "moveDirection");
			lua_getfield(m_lua, -1, "x");
			output.moveDirection.x = lua_tonumber(m_lua, -1);
			lua_pop(m_lua, 1);
			lua_getfield(m_lua, -1, "y");
			output.moveDirection.y = lua_tonumber(m_lua, -1);
			lua_pop(m_lua, 1);
			lua_pop(m_lua, 1);
			lua_getfield(m_lua, -1, "action");
			const char *temp=lua_tostring(m_lua, -1);
			output.action = (strcmp("shoot", lua_tostring(m_lua, -1))==0)?BotOutput::shoot:BotOutput::scan;
			lua_pop(m_lua, 1);
			lua_pop(m_lua, 1);
		}
	}
	lua_settop(m_lua, 0);
}

void LuaBot::result(bool won)
{
	lua_getglobal(m_lua, "result");
	if(lua_isfunction(m_lua, -1))
	{
		lua_pushboolean(m_lua, won);
		lua_call(m_lua, 1, 0);
	}
}
