#ifndef BOT_H
#define BOT_H
#include "bot_interface.h"
#include "kf/kf_random.h"

#ifdef BOT_EXPORTS
#define BOT_API __declspec(dllexport)
#else
#define BOT_API __declspec(dllimport)
#endif

class Max :public BotInterface27
{
public:
	Max();
	virtual ~Max();
	virtual void init(const BotInitialData &initialData, BotAttributes &attrib);
	virtual void update(const BotInput &input, BotOutput27 &output);
	virtual void result(bool won);
	virtual void bulletResult(bool hit);

	void Scan(BotOutput27 &output);

	kf::Vector2 TargetPrediction(kf::Vector2 enemyPos, kf::Vector2 myPos, kf::Vector2 enemyVelocity, float bulletSpeed);

	kf::Xor128		m_rand;					// Random number generator.
	BotInitialData	m_initialData;			// A copy of the initial map data.
	kf::Vector2		m_ourLastPos;			// Used to draw the trail behind the bot. Our pos from the last update.
	kf::Vector2		m_moveTarget;			// The location we are moving towards.
	float			m_scanAngle;			// The current angle we are scanning.
	kf::Vector2		m_currentEnemyPos;		// The most recent known enemy position.
	kf::Vector2		m_lastEnemyPos;			// The second most recent known enemy position.
	kf::Vector2		m_calculatedVelocity;
	int				m_lastEnemyUpdateCount; // The update count when we last saw an enemy.
	int				m_updateCount;			// The current update count (incremented by 1 every update).
	int				m_enemyScanCount;		// The number of successive scans that have found an enemy

	std::vector<VisibleThing> visibleBullets;
	std::vector<kf::Vector2> visibleBulletsPos1;
	std::vector<kf::Vector2> visibleBulletsPos2;
	std::vector<kf::Vector2> possibleHitPoints;
};


#endif