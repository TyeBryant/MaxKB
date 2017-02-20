#include "bot.h"
#include "time.h"

extern "C"
{
	BotInterface27 BOT_API *CreateBot27()
	{
		return new Max();
	}
}

Max::Max()
{
	m_rand(time(0)+(int)this);
}

Max::~Max()
{
}

void Max::init(const BotInitialData &initialData, BotAttributes &attrib)
{
	m_updateCount = 0;
	m_lastEnemyUpdateCount = -1;
	m_enemyScanCount = 0;
	m_initialData = initialData;
	attrib.health=1.0;
	attrib.motor=1.0;
	attrib.weaponSpeed=0.1;
	attrib.weaponStrength=1.0;
	
	m_ourLastPos.set(1, 0);
	m_moveTarget.set(m_rand() % (m_initialData.mapData.width - 2) + 1.5, m_rand() % (m_initialData.mapData.width - 2) + 1.5);
	m_scanAngle = 0;
}

void Max::update(const BotInput &input, BotOutput27 &output)
{
	bool eSpotted = false;

	if (m_updateCount == 0)
	{
		output.lines.clear();
		output.text.clear();
	}

	//Check Scan
	if (input.scanResult.size() > 0)
	{
		for (int i = 0; i < input.scanResult.size(); ++i)
		{
			if (input.scanResult[i].type == VisibleThing::e_robot)
			{
				m_lastEnemyPos = m_currentEnemyPos;
				m_currentEnemyPos = input.scanResult[i].position;
				m_calculatedVelocity = m_currentEnemyPos - m_lastEnemyPos;
				eSpotted = true;
				++m_enemyScanCount;

				break;
			}
			else if (input.scanResult[i].type == VisibleThing::e_bullet)
			{
				if (input.scanResult[i].e_bullet != visibleBullets[i].e_bullet)
				{
					visibleBullets.push_back(input.scanResult[i]);
					if (visibleBulletsPos1.size() > 0)
						visibleBulletsPos2.push_back(visibleBulletsPos1[i]);
					visibleBulletsPos1.push_back(input.scanResult[i].position - input.position);
				}
			}
		}
	}
	else
	{
		visibleBullets.clear();
		visibleBulletsPos1.clear();
		visibleBulletsPos2.clear();
	}

	//Movement
	if (visibleBullets.size() > 0)
	{
		for (int i = 0; i < visibleBullets.size(); ++i)
		{
			kf::Vector2 bulletVelocity = visibleBulletsPos1[i] - visibleBulletsPos2[i];
			for (int possibleHitPointsIndex = 0; possibleHitPointsIndex < 10; ++possibleHitPointsIndex)
			{
				possibleHitPoints.push_back(visibleBulletsPos1[i] + bulletVelocity * (possibleHitPointsIndex + 1));
			}
		}
		for (int i = 0; i < possibleHitPoints.size(); ++i)
			if (m_moveTarget.x == possibleHitPoints[i].x && m_moveTarget.y == possibleHitPoints[i].y)
			{
				m_moveTarget.set(m_rand() % (m_initialData.mapData.width - 2) + 1.5, m_rand() % (m_initialData.mapData.width - 2) + 1.5);
			}
	}
	else if (output.moveDirection.length() < 1)
	{
		m_moveTarget.set(m_rand() % (m_initialData.mapData.width - 2) + 1.5, m_rand() % (m_initialData.mapData.width - 2) + 1.5);
	}

	output.moveDirection = m_moveTarget - input.position;
	output.motor = 1.0;

	if (eSpotted)
	{
		kf::Vector2 estimatedEnemyPosition;
		if (m_lastEnemyUpdateCount > -1)
		{
			kf::Vector2 delta = m_currentEnemyPos - m_lastEnemyPos;
			estimatedEnemyPosition = m_currentEnemyPos + (delta / (m_updateCount - m_lastEnemyUpdateCount)) * 3;
			estimatedEnemyPosition = TargetPrediction(estimatedEnemyPosition, input.position, m_calculatedVelocity, input.bulletSpeed);
			Line lp;
			Line la;
			lp.start = m_currentEnemyPos;
			la.start = m_lastEnemyPos;
			lp.end = estimatedEnemyPosition;
			la.end = m_currentEnemyPos;
			lp.r = 1;
			la.r = 0;
			lp.g = 1;
			la.g = 1;
			lp.b = 1;
			la.b = 0;
			output.lines.push_back(lp);
			output.lines.push_back(la);
		}
		// Shooting
		if (m_enemyScanCount >= 3)
		{
			output.lookDirection = estimatedEnemyPosition;
			output.moveDirection = output.lookDirection;
			m_moveTarget = estimatedEnemyPosition;
			m_enemyScanCount = 0;
			output.action = BotOutput::shoot;
		}

		m_scanAngle -= m_initialData.scanFOV * 3;

		m_lastEnemyUpdateCount = m_updateCount;
	}
	else
	{
		// Scanning
		Scan(output);
	}

	m_updateCount++;
}

void Max::result(bool won)
{
}

void Max::bulletResult(bool hit)
{

}

kf::Vector2 Max::TargetPrediction(kf::Vector2 enemyPos, kf::Vector2 myPos, kf::Vector2 enemyVelocity, float bulletSpeed)
{
	// Given: ux, uy, vmag (projectile speed), Ax, Ay, Bx, By

	// Find the vector AB
	kf::Vector2 AB = enemyPos - myPos;

		// Normalize it
		AB.normalise();

		// Project u onto AB
		float uDotAB = enemyVelocity.dot(AB);
		float ujx = uDotAB * AB.x;
		float ujy = uDotAB * AB.y;

		// Subtract uj from u to get ui
		float uix = enemyVelocity.x - ujx;
		float uiy = enemyVelocity.y - ujy;

		// Set vi to ui (for clarity)
		float vix = uix;
		float viy = uiy;

		// Calculate the magnitude of vj
		float viMag = sqrt(vix * vix + viy * viy);
		float vjMag = sqrt(bulletSpeed * bulletSpeed - viMag * viMag);

		// Get vj by multiplying it's magnitude with the unit vector AB
		float vjx = AB.x * vjMag;
		float vjy = AB.y * vjMag;

		// Add vj and vi to get v
		kf::Vector2 v;
		v.x = vjx + vix;
		v.y = vjy + viy;

		return v;
}

void Max::Scan(BotOutput27 &output)
{
	m_scanAngle += m_initialData.scanFOV * 2;
	output.lookDirection.set(cos(m_scanAngle), sin(m_scanAngle));
	output.action = BotOutput::scan;
}