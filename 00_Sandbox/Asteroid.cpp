#include "Asteroid.h"
//constructor
Asteroid::Asteroid(vector3 pos, vector3 trg, float spd, float massIn) : Projectile(pos, trg, spd, massIn)
{
}

void Asteroid::moveAsteroid(float timeSpan)
{

	super::moveProjectile(timeSpan);
}

matrix4 Asteroid::getMatrix()
{
	return super::getMatrix();
}

bool Asteroid::isTargetReached()
{
	return super::isTargetReached();
}
