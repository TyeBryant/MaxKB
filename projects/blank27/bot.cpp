#include "bot.h"
#include "time.h"

extern "C"
{
	BotInterface27 BOT_API *CreateBot27()
	{
		return new Blank();
	}
}

Blank::Blank()
{
	m_rand(time(0)+(int)this);
}

Blank::~Blank()
{
}

void Blank::init(const BotInitialData &initialData, BotAttributes &attrib)
{
	m_initialData = initialData;
	attrib.health=1.0;
	attrib.motor=1.0;
	attrib.weaponSpeed=1.0;
	attrib.weaponStrength=1.0;
	//dir.set(m_rand.norm()*2.0 - 1.0, m_rand.norm()*2.0 - 1.0);
	dir.set(1, 0);
}

void Blank::update(const BotInput &input, BotOutput27 &output)
{
	output.moveDirection = dir;
	//output.moveDirection.set(1, 0);
	//output.moveDirection.set(m_rand.norm()*2.0-1.0, m_rand.norm()*2.0-1.0);
	output.motor = 1.0;
	output.lookDirection.set(0,1);
	output.action = BotOutput::scan;
	//output.spriteFrame = (output.spriteFrame+1)%2;
	output.text.clear();
	char buf[100];
	sprintf(buf, "%d", input.health);
	output.text.push_back(TextMsg(buf, input.position - kf::Vector2(0.0f, 1.0f), 0.0f, 0.7f, 1.0f,80));
}

void Blank::result(bool won)
{
}

void Blank::bulletResult(bool hit)
{

}
