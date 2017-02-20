#include "bot.h"
#include "time.h"
#include "windows.h"
#include "xinput.h"

extern "C"
{
	BotInterface BOT_API *CreateBot()
	{
		return new ManualBot();
	}
}

ManualBot::ManualBot()
{
	m_rand(time(0));
}

ManualBot::~ManualBot()
{
	int dummy=0;
}

void ManualBot::init(const BotInitialData &initialData, BotAttributes &attrib)
{
	m_lookDir.set(0,1);
	attrib.motor          = 10.0f;
	attrib.health         = 1.0f;
	attrib.weaponSpeed    = 1.0f;
	attrib.weaponStrength = 1.0f;
	int x = 10;
	int y = 5;
	if (initialData.mapData.data[x + y*initialData.mapData.width].wall)
	{
	}
}

void ManualBot::update(const BotInput &input, BotOutput &output)
{
	//output.m_moveDirection.set(m_rand.norm()*2.0-1.0, m_rand.norm()*2.0-1.0);
	output.motor = 1.0;

	XINPUT_STATE xbox;
	XInputGetState(0,&xbox);
	kf::Vector2 vel;
	vel.x = xbox.Gamepad.sThumbLX/32767.0f;
	vel.y = -xbox.Gamepad.sThumbLY/32767.0f;
	if(vel.length()<0.2) 
		vel.set(0,0);
	output.moveDirection = vel;
	output.motor = vel.lengthSquared();

	if(xbox.Gamepad.wButtons & XINPUT_GAMEPAD_B)
	{
		int dummy=0;
	}
	kf::Vector2 aim(0,0);
	aim.x = xbox.Gamepad.sThumbRX/32767.0f;
	aim.y = -xbox.Gamepad.sThumbRY/32767.0f;
	if(aim.length()>0.2) 
		m_lookDir.set(aim);
	output.lookDirection = m_lookDir;
	output.action = BotOutput::scan;
	if(xbox.Gamepad.bRightTrigger>127)
	{
		output.action = BotOutput::shoot;
	}
}

void ManualBot::result(bool won)
{
	int dummy=0;
}
