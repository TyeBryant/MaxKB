#ifndef BOT_H
#define BOT_H
#include "bot_interface.h"
#include "kf/kf_random.h"

#ifdef BOT_EXPORTS
#define BOT_API __declspec(dllexport)
#else
#define BOT_API __declspec(dllimport)
#endif

class BottyMcBotface:public BotInterface27
{
public:
	BottyMcBotface();
	virtual ~BottyMcBotface();
	virtual void init(const BotInitialData &initialData, BotAttributes &attrib);
	virtual void update(const BotInput &input, BotOutput27 &output);
	virtual void result(bool won);
	virtual void bulletResult(bool hit);

	kf::Xor128		m_rand;					// Random number generator.
	BotInitialData	m_initialData;			// A copy of the initial map data.
	kf::Vector2		m_ourLastPos;			// Used to draw the trail behind the bot. Our pos from the last update.
	kf::Vector2		m_moveTarget;			// The location we are moving towards.
	float			m_scanAngle;			// The current angle we are scanning.
	kf::Vector2		m_currentEnemyPos;		// The most recent known enemy position.
	kf::Vector2		m_lastEnemyPos;			// The second most recent known enemy position.
	int				m_lastEnemyUpdateCount; // The update count when we last saw an enemy.
	int				m_updateCount;			// The current update count (incremented by 1 every update).
};


#endif