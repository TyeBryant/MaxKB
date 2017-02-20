#include "bot.h"
#include "time.h"

extern "C"
{
	BotInterface27 BOT_API *CreateBot27()
	{
		return new BottyMcBotface();
	}
}

BottyMcBotface::BottyMcBotface()
{
	m_rand(time(0)+(int)this);
}

BottyMcBotface::~BottyMcBotface()
{
}

void BottyMcBotface::init(const BotInitialData &initialData, BotAttributes &attrib)
{
	m_updateCount = 0;
	m_lastEnemyUpdateCount = -1;
	m_initialData = initialData;
	attrib.health=1.0;
	attrib.motor=1.0;
	attrib.weaponSpeed=1.0;
	attrib.weaponStrength=1.0;
	//dir.set(m_rand.norm()*2.0 - 1.0, m_rand.norm()*2.0 - 1.0);
	m_ourLastPos.set(1, 0);
	m_moveTarget.set(m_rand() % (m_initialData.mapData.width - 2) + 1.5, m_rand() % (m_initialData.mapData.width - 2) + 1.5);
	m_scanAngle = 0;
}

void BottyMcBotface::update(const BotInput &input, BotOutput27 &output)
{
	bool spotted = false;
	
	// First update
	if (m_updateCount == 0)
	{
		output.lines.clear();
		output.text.clear();
	}

	if (input.scanResult.size() > 0)
	{
		for (int i = 0;i < input.scanResult.size();++i)
		{
			if (input.scanResult[i].type == VisibleThing::e_robot)
			{
				m_currentEnemyPos = input.scanResult[i].position;
				spotted = true;

				break;
			}
		}
	}


// Movement
	output.moveDirection = m_moveTarget - input.position;
	if (output.moveDirection.length() < 2)
	{
		m_moveTarget.set(m_rand() % (m_initialData.mapData.width - 2) + 1.5, m_rand() % (m_initialData.mapData.width - 2) + 1.5);
	}
	output.motor = 1.0;
	

	if (spotted)
	{
		kf::Vector2 estimatedEnemyPosition = m_currentEnemyPos;
		if (m_lastEnemyUpdateCount>-1)
		{
			kf::Vector2 delta = m_currentEnemyPos - m_lastEnemyPos;
			estimatedEnemyPosition = m_currentEnemyPos + (delta / (m_updateCount - m_lastEnemyUpdateCount)) * 5;
			/*
			Line l;
			l.start = m_currentEnemyPos;
			l.end = estimatedEnemyPosition;
			l.r = 1;
			l.g = 1;
			l.b = 1;
			output.lines.push_back(l);
			*/
		}
		// Shooting
		output.lookDirection = estimatedEnemyPosition - input.position;
		output.moveDirection = output.lookDirection;
		m_moveTarget = estimatedEnemyPosition;
		output.action = BotOutput::shoot;
		m_scanAngle -= m_initialData.scanFOV * 3;
	}
	else
	{
		// Scanning
		m_scanAngle += m_initialData.scanFOV * 2;
		output.lookDirection.set(cos(m_scanAngle), sin(m_scanAngle));
		output.action = BotOutput::scan;
	}

	// Debugging lines. Render a trail behind us.
	/*
	if (m_updateCount > 0)
	{
		Line l;
		l.start = input.position;
		l.end = m_ourLastPos;
		l.r = 1;
		l.g = 0;
		l.b = 0;
		output.lines.push_back(l);
	}
	*/
	// How to change sprite frames (if the sprite has more than 1)
	//output.spriteFrame = (output.spriteFrame+1)%2;


	// How to render text on the screen.
	//output.text.clear();
	//char buf[100];
	//sprintf(buf, "%d", input.health);
	//output.text.push_back(TextMsg(buf, input.position - kf::Vector2(0.0f, 1.0f), 0.0f, 0.7f, 1.0f, 80));
	

	// Remember values for the next update.
	m_ourLastPos = input.position;
	if (spotted)
	{
		m_lastEnemyUpdateCount = m_updateCount;
		m_lastEnemyPos = m_currentEnemyPos;
	}
	m_updateCount++;
}

void BottyMcBotface::result(bool won)
{
}

void BottyMcBotface::bulletResult(bool hit)
{

}
