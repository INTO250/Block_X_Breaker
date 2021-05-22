#pragma once

#include "cocos2d.h"

class Ball : public cocos2d::Sprite
{
public:
	static Ball* createBall(const char* filename);
	int type; // 种类
	bool existence; //是否存在
	int speedX;
	int speedY;
};