#pragma once

#include "cocos2d.h"

class Ball : public cocos2d::Sprite
{
public:
	static Ball* createBall(const char* filename);
	bool existence; //ÊÇ·ñ´æÔÚ
	
};