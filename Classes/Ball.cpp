#include "Ball.h"

USING_NS_CC;

Ball* Ball::createBall(const char* filename)
{
	Ball* rua = new Ball();
	if (rua && rua->initWithFile(filename))
	{
		rua->autorelease();
		return rua;
	}
	CC_SAFE_DELETE(rua);
	return nullptr;
}