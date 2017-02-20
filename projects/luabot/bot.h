#ifndef BOT_H
#define BOT_H
#include "bot_interface.h"
#include "kf/kf_random.h"
#include "lua/lua.hpp"

#ifdef BOT_EXPORTS
#define BOT_API __declspec(dllexport)
#else
#define BOT_API __declspec(dllimport)
#endif

class LuaBot:public BotInterface
{
public:
	LuaBot();
	virtual ~LuaBot();
	virtual void init(const BotInitialData &initialData, BotAttributes &attrib);
	virtual void update(const BotInput &input, BotOutput &output);
	virtual void result(bool won);
	lua_State	*m_lua;
	kf::Xor128 m_rand;
};


#endif